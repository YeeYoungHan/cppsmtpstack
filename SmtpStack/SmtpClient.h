/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#ifndef _SMTP_CLIENT_H_
#define _SMTP_CLIENT_H_

#include <string>
#include "SipTcp.h"
#include "SmtpResponse.h"

class CSmtpClient
{
public:
	CSmtpClient();
	~CSmtpClient();

	bool Connect( const char * pszServerIp, int iServerPort, const char * pszUserId, const char * pszPassWord );
	void Close( );

	bool Send( const char * pszFrom, const char * pszTo, const char * pszSubject, const char * pszData );

private:
	bool Send( std::string & strRequest, CSmtpResponse & clsResponse, int iWantCode = 0 );
	bool Send( const char * pszRequest, CSmtpResponse & clsResponse, int iWantCode = 0 );
	bool Recv( CSmtpResponse & clsResponse );

	std::string m_strServerIp;
	int					m_iServerPort;
	std::string m_strUserId;
	std::string m_strPassWord;
	int					m_iTimeout;

	Socket			m_hSocket;
};

#endif
