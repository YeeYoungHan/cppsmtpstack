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

#include "SipPlatformDefine.h"
#include "SmtpClient.h"
#include "Log.h"
#include "MemoryDebug.h"

CSmtpClient::CSmtpClient() : m_iServerPort(25), m_iTimeout(10), m_hSocket(INVALID_SOCKET)
{
}

CSmtpClient::~CSmtpClient()
{
	Close();
}

bool CSmtpClient::Connect( const char * pszServerIp, int iServerPort, const char * pszUserId, const char * pszPassWord )
{
	if( m_hSocket != INVALID_SOCKET )
	{
		CLog::Print( LOG_ERROR, "%s m_hSocket != INVALID_SOCKET", __FUNCTION__ );
		return false;
	}

	m_strServerIp = pszServerIp;
	m_iServerPort = iServerPort;
	m_strUserId = pszUserId;
	m_strPassWord = pszPassWord;

	m_hSocket = TcpConnect( pszServerIp, iServerPort, m_iTimeout );
	if( m_hSocket == INVALID_SOCKET )
	{
		CLog::Print( LOG_ERROR, "%s TcpConnect(%s:%d) error(%d)", __FUNCTION__, pszServerIp, iServerPort, GetError() );
		return false;
	}

	return true;
}

void CSmtpClient::Close( )
{
	if( m_hSocket != INVALID_SOCKET )
	{
		closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
	}
}

bool CSmtpClient::Send( const char * pszFrom, const char * pszTo, const char * pszSubject, const char * pszData )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		CLog::Print( LOG_ERROR, "%s m_hSocket = INVALID_SOCKET", __FUNCTION__ );
		return false;
	}

	std::string strSendBuf;
	char	szRecvBuf[8192];
	int		iSendBufLen;

	strSendBuf = "MAIL FROM:";
	strSendBuf.append( "<" );
	strSendBuf.append( pszFrom );
	strSendBuf.append( ">" );

	iSendBufLen = (int)strSendBuf.length();

	if( TcpSend( m_hSocket, strSendBuf.c_str(), iSendBufLen ) != iSendBufLen )
	{
		CLog::Print( LOG_ERROR, "%s TcpSend(%s) error(%d)", __FUNCTION__, strSendBuf.c_str(), GetError() );
		return false;
	}

	TcpRecv( m_hSocket, szRecvBuf, sizeof(szRecvBuf), m_iTimeout );

	return true;
}
