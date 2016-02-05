//*********************************************************************************************************************
// HttpCurl.cpp
//
// Copyright (c) 2013 ELECTRIC POWER RESEARCH INSTITUTE, INC. All rights reserved.
//
// OpenADR ("this software") is licensed under BSD 3-Clause license.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
// following conditions are met:
//
// *    Redistributions of source code must retain the above copyright  notice, this list of conditions and
//      the following disclaimer.
//
// *    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
//      the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// *    Neither the name of the Electric Power Research Institute, Inc. ("EPRI") nor the names of its contributors
//      may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL EPRI BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
#include "HttpCurl.h"

bool HttpCurl::m_curlInitialized = false;
int HttpCurl::m_instanceCount = 0;

/********************************************************************************/

size_t writeFunction(char *ptr, size_t size, size_t nmemb, void *userdata);
size_t readFunction(void *ptr, size_t size, size_t nmemb, void *userdata);

/********************************************************************************/

HttpCurl::HttpCurl(bool useSSL, string caFilePath, int sslVersion)
{
	CURLcode result;

	m_message[0] = 0;

	m_requestHeaders = NULL;

	// TODO: this check is not threadsafe; use a lock
	if (!m_curlInitialized)
	{

		if ((result = curl_global_init(CURL_GLOBAL_ALL)) != 0)
		{
			CurlException ex(result, m_message);
			throw ex;
		}
	}

	// init curl
	// how do we get the error code when initialiation failed?
	if ((m_curl = curl_easy_init()) == NULL)
	{
		CurlException ex((CURLcode)0, m_message);
		throw ex;
	}

	// set error buffer
	if ((result = curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, m_message)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

#ifdef _CURL_DEBUG
	// enable verbose output
	if ((result = curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// enable header output
	if ((result = curl_easy_setopt(m_curl, CURLOPT_HEADER, 1)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// redirect output to curl_output.txt
	FILE *fp = fopen("curl_output.txt", "a");

	if (fp != NULL)
	{
		if ((result = curl_easy_setopt(m_curl, CURLOPT_STDERR, fp)) != 0)
		{
			CurlException ex(result, m_message);
			throw ex;
		}
	}
	else
		cout << "failed to open file curl_output.txt: " << strerror(errno);

#endif

	if (useSSL)
	{
		// set the ca file path
		if (caFilePath != "")
		{
			if ((result = curl_easy_setopt(m_curl, CURLOPT_ISSUERCERT, caFilePath.c_str())) != 0)
			{
				CurlException ex(result, m_message);
				throw ex;
			}
		}

		// force SSL 1.2 (requires libcurl >= 7.34.0)
		if ((result = curl_easy_setopt(m_curl, CURLOPT_SSLVERSION, sslVersion)) != 0)
		{
			CurlException ex(result, m_message);
			throw ex;
		}
	}

	// all requests will be posts
	if ((result = curl_easy_setopt(m_curl, CURLOPT_POST, 1L)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// set the write function
	if ((result = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &::writeFunction)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// set the write parameter
	if ((result = curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}
	// set the write header
	if ((result = curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// set the read function
	if ((result = curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, &::readFunction)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// set the read parameter
	if ((result = curl_easy_setopt(m_curl, CURLOPT_READDATA, this)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	m_requestHeaders = curl_slist_append(m_requestHeaders, "Content-Type: application/xml");

	// remove header: Expect: 100-continue
	m_requestHeaders = curl_slist_append(m_requestHeaders, "Expect:");

	m_instanceCount++;
}

/********************************************************************************/

HttpCurl::~HttpCurl()
{
	if (m_curl != NULL)
		curl_easy_cleanup(m_curl);

	m_instanceCount--;

	// TODO: not thread safe; use a lock
	if (m_instanceCount == 0)
	{
		curl_global_cleanup();
	}

	if (m_requestHeaders != NULL)
		curl_slist_free_all(m_requestHeaders);
}

/********************************************************************************/

size_t HttpCurl::readFunction(void *ptr, size_t size, size_t nmemb)
{
	return m_sendBuffer.send((char *)ptr, size * nmemb);
}

/********************************************************************************/

size_t readFunction(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	HttpCurl *httpCurl = (HttpCurl *)userdata;

	return httpCurl->readFunction(ptr, size, nmemb);
}

/********************************************************************************/

size_t HttpCurl::writeFunction(char *ptr, size_t size, size_t nmemb)
{
	return m_receiveBuffer.receive(ptr, size * nmemb);
}

/********************************************************************************/

size_t writeFunction(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	HttpCurl *httpCurl = (HttpCurl *)userdata;

	return httpCurl->writeFunction(ptr, size, nmemb);
}

/********************************************************************************/

bool HttpCurl::post(string url, string content)
{
	CURLcode result;

	m_sendBuffer.initialize(content);
	m_receiveBuffer.clear();

	// set the url
	if ((result = curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str())) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	if ((result = curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, content.length())) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// set the application type header
	if ((result = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_requestHeaders)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	// initiate the request
	if ((result = curl_easy_perform(m_curl)) != 0)
	{
		CurlException ex(result, m_message);
		throw ex;
	}

	m_receiveBuffer.parse(m_headers, m_cookies, m_responseBody, m_httpVersion, m_httpResponseCode, m_httpResponseMessage);

	return true;
}

/********************************************************************************/

string HttpCurl::getRequestBody()
{
	return m_sendBuffer.content();
}

/********************************************************************************/

string HttpCurl::getResponseBody()
{
	return m_responseBody;
}

/********************************************************************************/

string HttpCurl::getResponseCode()
{
	return m_httpResponseCode;
}

/********************************************************************************/

string HttpCurl::getResponseMessage()
{
	return m_httpResponseMessage;
}

/********************************************************************************/

time_t HttpCurl::getResponseTime()
{
	return 0;
}

/********************************************************************************/

string HttpCurl::getServerDate()
{
	return m_headers["Date"];
}
