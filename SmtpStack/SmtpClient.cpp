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
#include "Base64.h"
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

	CSmtpResponse clsResponse;

	if( Recv( clsResponse ) == false )
	{
		CLog::Print( LOG_ERROR, "%s recv first response error", __FUNCTION__ );
		Close();
		return false;
	}

	if( clsResponse.m_iCode != 220 )
	{
		CLog::Print( LOG_ERROR, "%s reply code(%d) != 220", __FUNCTION__, clsResponse.m_iCode );
		Close();
		return false;
	}

	if( Send( "EHLO", clsResponse, 250 ) == false )
	{
		CLog::Print( LOG_ERROR, "%s EHLO error", __FUNCTION__ );
		Close();
		return false;
	}

	STRING_LIST::iterator itSL;
	STRING_LIST clsAuthList;

	for( itSL = clsResponse.m_clsReplyList.begin(); itSL != clsResponse.m_clsReplyList.end(); ++itSL )
	{
		if( !strncmp( itSL->c_str(), "AUTH", 4 ) )
		{
			SplitString( itSL->c_str(), clsAuthList, ' ' );
			if( clsAuthList.empty() == false ) break;
		}
	}

	if( SearchStringList( clsAuthList, "PLAIN" ) )
	{
		if( Send( "AUTH PLAIN", clsResponse, 334 ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN error", __FUNCTION__ );
			Close();
			return false;
		}

		std::string strSendBuf;

		strSendBuf.clear();
		strSendBuf.append( "\0", 1 );
		strSendBuf.append( pszUserId );
		strSendBuf.append( "\0", 1 );
		strSendBuf.append( pszPassWord );

		int iLen = GetBase64EncodeLength( strSendBuf.length() ) + 10;
		char * pszBuf = (char *)malloc( iLen );
		if( pszBuf == NULL )
		{
			return false;
		}

		Base64Encode( strSendBuf.c_str(), strSendBuf.length(), pszBuf, iLen );
		
		bool bRes = Send( pszBuf, clsResponse, 235 );
		free( pszBuf );

		if( bRes == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN error", __FUNCTION__ );
			Close();
			return false;
		}
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
	CSmtpResponse clsResponse;

	strSendBuf = "MAIL FROM:";
	strSendBuf.append( "<" );
	strSendBuf.append( pszFrom );
	strSendBuf.append( ">" );

	if( Send( strSendBuf, clsResponse ) == false ) return false;

	return true;
}

bool CSmtpClient::Send( std::string & strRequest, CSmtpResponse & clsResponse, int iWantCode )
{
	strRequest.append( "\r\n" );

	int iSendLen = strRequest.length();

	CLog::Print( LOG_NETWORK, "TcpSend(%s:%d) [%s]", m_strServerIp.c_str(), m_iServerPort, strRequest.c_str() );

	if( TcpSend( m_hSocket, strRequest.c_str(), iSendLen ) != iSendLen )
	{
		CLog::Print( LOG_ERROR, "%s TcpSend(%s) error(%d)", __FUNCTION__, strRequest.c_str(), GetError() );
		return false;
	}

	if( Recv( clsResponse ) == false ) return false;

	if( clsResponse.m_iCode != iWantCode )
	{
		CLog::Print( LOG_ERROR, "%s reply code(%d) != want code(%d)", __FUNCTION__, clsResponse.m_iCode, iWantCode );
		return false;
	}

	return true;
}

bool CSmtpClient::Send( const char * pszRequest, CSmtpResponse & clsResponse, int iWantCode )
{
	std::string strRequest = pszRequest;

	return Send( strRequest, clsResponse, iWantCode );
}

bool CSmtpClient::Recv( CSmtpResponse & clsResponse )
{
	std::string strRecvBuf;
	char	szRecvBuf[8192];
	int n;

	while( 1 )
	{
		n = TcpRecv( m_hSocket, szRecvBuf, sizeof(szRecvBuf), m_iTimeout );
		if( n <= 0 )
		{
			CLog::Print( LOG_ERROR, "%s TcpRecv(%s) error(%d)", __FUNCTION__, strRecvBuf.c_str(), GetError() );
			return false;
		}

		CLog::Print( LOG_NETWORK, "TcpRecv(%s:%d) [%.*s]", m_strServerIp.c_str(), m_iServerPort, n, szRecvBuf );

		strRecvBuf.append( szRecvBuf, n );
		if( clsResponse.Parse( strRecvBuf.c_str(), strRecvBuf.length() ) > 0 )
		{
			break;
		}
	}

	return true;
}
