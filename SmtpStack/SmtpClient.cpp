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
#include "TimeString.h"
#include "MemoryDebug.h"

CSmtpClient::CSmtpClient() : m_iServerPort(25), m_bUseTls(false)
	,	m_hSocket(INVALID_SOCKET), m_psttSsl(NULL), m_iTimeout(10)
{
}

CSmtpClient::~CSmtpClient()
{
	Close();
}

bool CSmtpClient::SetServer( const char * pszServerIp, int iServerPort, bool bUseTls )
{
	if( pszServerIp == NULL || iServerPort <= 0 || iServerPort > 65535 )
	{
		return false;
	}

	m_strServerIp = pszServerIp;
	m_iServerPort = iServerPort;
	m_bUseTls = bUseTls;

	return true;
}

bool CSmtpClient::SetUser( const char * pszUserId, const char * pszPassWord )
{
	m_strUserId = pszUserId;
	m_strPassWord = pszPassWord;

	return true;
}

bool CSmtpClient::SetFrom( const char * pszEmailFrom )
{
	m_strEmailFrom = pszEmailFrom;

	return true;
}

bool CSmtpClient::SetTo( const char * pszEmailTo )
{
	m_strEmailTo = pszEmailTo;

	return true;
}

bool CSmtpClient::SetSubject( const char * pszSubject )
{
	m_strSubject = pszSubject;

	return true;
}

bool CSmtpClient::SetContent( const char * pszContent )
{
	m_strContent = pszContent;

	return true;
}

bool CSmtpClient::SetAttachFile( const char * pszFileName )
{
	m_strAttachFileName = pszFileName;

	return true;
}

/**
 * @ingroup SmtpStack
 * @brief SMTP 서버에 TCP/TLS 세션을 연결한 후, SMTP 로그인을 수행한다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSmtpClient::Connect( )
{
	if( m_hSocket != INVALID_SOCKET )
	{
		CLog::Print( LOG_ERROR, "%s m_hSocket != INVALID_SOCKET", __FUNCTION__ );
		return false;
	}

	m_hSocket = TcpConnect( m_strServerIp.c_str(), m_iServerPort, m_iTimeout );
	if( m_hSocket == INVALID_SOCKET )
	{
		CLog::Print( LOG_ERROR, "%s TcpConnect(%s:%d) error(%d)", __FUNCTION__, m_strServerIp.c_str(), m_iServerPort, GetError() );
		return false;
	}

	if( m_bUseTls )
	{
		SSLClientStart();
		if( SSLConnect( m_hSocket, &m_psttSsl ) == false )
		{
			CLog::Print( LOG_ERROR, "%s TlsConnect(%s:%d) error", __FUNCTION__, m_strServerIp.c_str(), m_iServerPort );
			Close();
			return false;
		}
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

		std::string strData, strSendBuf;

		strData.append( "\0", 1 );
		strData.append( m_strUserId );
		strData.append( "\0", 1 );
		strData.append( m_strPassWord );

		if( Base64Encode( strData.c_str(), strData.length(), strSendBuf ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN base64 error", __FUNCTION__ );
			Close();
			return false;
		}
		
		if( Send( strSendBuf, clsResponse, 235 ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN error", __FUNCTION__ );
			Close();
			return false;
		}
	}
	else if( SearchStringList( clsAuthList, "LOGIN" ) )
	{
		if( Send( "AUTH LOGIN", clsResponse, 334 ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN error", __FUNCTION__ );
			Close();
			return false;
		}

		std::string strBase64Id, strBase64Pw;

		if( Base64Encode( m_strUserId.c_str(), (int)m_strUserId.length(), strBase64Id ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN base64 id error", __FUNCTION__ );
			Close();
			return false;
		}

		if( Base64Encode( m_strPassWord.c_str(), (int)m_strPassWord.length(), strBase64Pw ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN base64 pw error", __FUNCTION__ );
			Close();
			return false;
		}

		if( Send( strBase64Id, clsResponse, 334 ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH LOGIN id error", __FUNCTION__ );
			Close();
			return false;
		}

		if( Send( strBase64Pw, clsResponse, 235 ) == false )
		{
			CLog::Print( LOG_ERROR, "%s AUTH PLAIN error", __FUNCTION__ );
			Close();
			return false;
		}
	}

	return true;
}

/**
 * @ingroup SmtpStack
 * @brief SMTP 세션을 종료한다.
 */
void CSmtpClient::Close( )
{
	if( m_psttSsl )
	{
		SSLClose( m_psttSsl );
		m_psttSsl = NULL;
	}

	if( m_hSocket != INVALID_SOCKET )
	{
		closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
	}
}

/**
 * @ingroup SmtpStack
 * @brief SMTP 메일을 전송한다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSmtpClient::Send( )
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
	strSendBuf.append( m_strEmailFrom );
	strSendBuf.append( ">" );

	if( Send( strSendBuf, clsResponse, 250 ) == false ) 
	{
		CLog::Print( LOG_ERROR, "%s MAIL FROM error", __FUNCTION__ );
		return false;
	}

	strSendBuf = "RCPT TO:";
	strSendBuf.append( "<" );
	strSendBuf.append( m_strEmailTo );
	strSendBuf.append( ">" );

	if( Send( strSendBuf, clsResponse, 250 ) == false ) 
	{
		CLog::Print( LOG_ERROR, "%s RCPT TO error", __FUNCTION__ );
		return false;
	}

	if( Send( "DATA", clsResponse, 354 ) == false )
	{
		CLog::Print( LOG_ERROR, "%s DATA error", __FUNCTION__ );
		return false;
	}

	strSendBuf = "Subject: ";
	strSendBuf.append( m_strSubject );
	strSendBuf.append( "\r\n" );

	strSendBuf.append( "From: " );
	strSendBuf.append( m_strEmailFrom );
	strSendBuf.append( "\r\n" );

	strSendBuf.append( "To: " );
	strSendBuf.append( m_strEmailTo );
	strSendBuf.append( "\r\n" );

	char szDate[31];

	GetSmtpDateString( szDate, sizeof(szDate) );
	strSendBuf.append( "Date: " );
	strSendBuf.append( szDate );
	strSendBuf.append( "\r\n\r\n" );

	strSendBuf.append( m_strContent );
	strSendBuf.append( "\r\n." );

	if( Send( strSendBuf, clsResponse, 250 ) == false )
	{
		CLog::Print( LOG_ERROR, "%s DATA body error", __FUNCTION__ );
		return false;
	}

	return true;
}

/**
 * @ingroup SmtpStack
 * @brief SMTP 서버로 데이터를 전송한다.
 * @param strRequest	SMTP 서버로 전송할 데이터
 * @param clsResponse [out] 응답 메시지를 저장한 변수
 * @param iWantCode		원하는 응답 코드
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSmtpClient::Send( std::string & strRequest, CSmtpResponse & clsResponse, int iWantCode )
{
	strRequest.append( "\r\n" );

	int iSendLen = strRequest.length();

	if( m_psttSsl )
	{
		CLog::Print( LOG_NETWORK, "TlsSend(%s:%d) [%s]", m_strServerIp.c_str(), m_iServerPort, strRequest.c_str() );

		if( SSLSend( m_psttSsl, strRequest.c_str(), iSendLen ) != iSendLen )
		{
			CLog::Print( LOG_ERROR, "%s TlsSend(%s) error", __FUNCTION__, strRequest.c_str() );
			return false;
		}
	}
	else
	{
		CLog::Print( LOG_NETWORK, "TcpSend(%s:%d) [%s]", m_strServerIp.c_str(), m_iServerPort, strRequest.c_str() );

		if( TcpSend( m_hSocket, strRequest.c_str(), iSendLen ) != iSendLen )
		{
			CLog::Print( LOG_ERROR, "%s TcpSend(%s) error(%d)", __FUNCTION__, strRequest.c_str(), GetError() );
			return false;
		}
	}

	if( Recv( clsResponse ) == false ) return false;

	if( clsResponse.m_iCode != iWantCode )
	{
		CLog::Print( LOG_ERROR, "%s reply code(%d) != want code(%d)", __FUNCTION__, clsResponse.m_iCode, iWantCode );
		return false;
	}

	return true;
}

/**
 * @ingroup SmtpStack
 * @brief SMTP 서버로 데이터를 전송한다.
 * @param pszRequest	SMTP 서버로 전송할 데이터
 * @param clsResponse [out] 응답 메시지를 저장한 변수
 * @param iWantCode		원하는 응답 코드
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSmtpClient::Send( const char * pszRequest, CSmtpResponse & clsResponse, int iWantCode )
{
	std::string strRequest = pszRequest;

	return Send( strRequest, clsResponse, iWantCode );
}

/**
 * @ingroup SmtpStack
 * @brief SMTP 서버에서 응답 메시지를 수신한다.
 * @param clsResponse [out] 응답 메시지를 저장한 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSmtpClient::Recv( CSmtpResponse & clsResponse )
{
	std::string strRecvBuf;
	char	szRecvBuf[8192];
	int n;

	while( 1 )
	{
		if( m_psttSsl )
		{
			n = SSLRecv( m_psttSsl, szRecvBuf, sizeof(szRecvBuf) );
			if( n <= 0 )
			{
				CLog::Print( LOG_ERROR, "%s TlsRecv(%s) error", __FUNCTION__, strRecvBuf.c_str() );
				return false;
			}

			CLog::Print( LOG_NETWORK, "TlsRecv(%s:%d) [%.*s]", m_strServerIp.c_str(), m_iServerPort, n, szRecvBuf );
		}
		else
		{
			n = TcpRecv( m_hSocket, szRecvBuf, sizeof(szRecvBuf), m_iTimeout );
			if( n <= 0 )
			{
				CLog::Print( LOG_ERROR, "%s TcpRecv(%s) error(%d)", __FUNCTION__, strRecvBuf.c_str(), GetError() );
				return false;
			}

			CLog::Print( LOG_NETWORK, "TcpRecv(%s:%d) [%.*s]", m_strServerIp.c_str(), m_iServerPort, n, szRecvBuf );
		}

		strRecvBuf.append( szRecvBuf, n );
		if( clsResponse.Parse( strRecvBuf.c_str(), strRecvBuf.length() ) > 0 )
		{
			break;
		}
	}

	return true;
}
