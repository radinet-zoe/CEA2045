//*********************************************************************************************************************
// ReportWrapperList.cpp
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
#include "ReportWrapperList.h"
#include <iostream>

ReportWrapperList::ReportWrapperList()
{
}

/********************************************************************************/

ReportWrapperList::~ReportWrapperList()
{
	clear(true);
}

/********************************************************************************/

void ReportWrapperList::addReportWrapper(ReportWrapper *reportWrapper)
{
	if (m_reportSepcifierIDtoReport.find(reportWrapper->reportSpecifierID()) != m_reportSepcifierIDtoReport.end())
	{
		OadrException exception(OadrException::REPORT_SPEC_EXISTS);
		throw exception;
	}

	m_reports.push_back(reportWrapper);

	m_reportSepcifierIDtoReport[reportWrapper->reportSpecifierID()] = reportWrapper;
}

/********************************************************************************/

ReportWrapper *ReportWrapperList::getReportWrapper(string reportSpecifierID)
{
	if (m_reportSepcifierIDtoReport.find(reportSpecifierID) == m_reportSepcifierIDtoReport.end())
	{
		OadrException exception(OadrException::REPORT_SPEC_NON_EXISTENT);
		throw exception;
	}

	return m_reportSepcifierIDtoReport[reportSpecifierID];
}

/********************************************************************************/

void ReportWrapperList::addReportDescriptionsToSequence(oadrRegisterReportType::oadrReport_sequence &sequence)
{
	vector<ReportWrapper*>::iterator itr;

	for (itr = m_reports.begin(); itr != m_reports.end(); itr++)
		sequence.push_back((*itr)->generateReportDescription());
}

/********************************************************************************/

void ReportWrapperList::addReportIntervalsToSequence(oadrUpdateReportType::oadrReport_sequence &sequence, time_t dtstart, string reportRequestID, time_t createdDateTime)
{
	vector<ReportWrapper*>::iterator itr;

	for (itr = m_reports.begin(); itr != m_reports.end(); itr++)
		sequence.push_back((*itr)->generateReport(dtstart, reportRequestID, createdDateTime));
}

/********************************************************************************/

void ReportWrapperList::clear(bool deleteReportWrapper)
{
	m_reportSepcifierIDtoReport.clear();

	if (!deleteReportWrapper)
	{
		m_reports.clear();
		return;
	}

	while (m_reports.size() > 0)
	{
		ReportWrapper *reportWrapper = m_reports.back();

		delete (reportWrapper);

		m_reports.pop_back();
	}
}
