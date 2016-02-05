/*
 * Registerparty.h
 *
 *  Created on: 25-Aug-2014
 *      Author: dinesh
 */

#ifndef REGISTERPARTY_H_
#define REGISTERPARTY_H_

#include <oadr/ven/VEN2b.h>

using namespace std;

class registerParty
{
private:
	string venID;
	string registrationID;

public:
	registerParty(string venID="",
				  string registrationID="");

	bool RegisterParty(VEN2b *ven);

	bool ifregistered();

	const string& getRegistrationId() const {
		return registrationID;
	}

	void setRegistrationId(const string& registrationId) {
		registrationID = registrationId;
	}

	const string& getVenId() const {
		return venID;
	}

	void setVenId(const string& venId) {
		venID = venId;
	}
};

#endif /* REGISTERPARTY_H_ */
