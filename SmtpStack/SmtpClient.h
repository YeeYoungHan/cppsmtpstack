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
#include "TlsFunction.h"

/**
 * @ingroup SmtpStack
 * @brief SMTP 클라이언트 - SMTP 서버로 메일을 전송한다.
 */
class CSmtpClient
{
public:
	CSmtpClient();
	~CSmtpClient();

	bool SetServer( const char * pszServerIp, int iServerPort, bool bUseTls = false );
	bool SetUser( const char * pszUserId, const char * pszPassWord );
	bool SetFrom( const char * pszEmailFrom );
	bool SetTo( const char * pszEmailTo );
	bool SetSubject( const char * pszSubject );
	bool SetContent( const char * pszContent );
	bool SetAttachFile( const char * pszFileName );

	bool Connect( );
	void Close( );

	bool Send( );

private:

	bool Send( std::string & strRequest, CSmtpResponse & clsResponse, int iWantCode = 0 );
	bool Send( const char * pszRequest, CSmtpResponse & clsResponse, int iWantCode = 0 );
	bool Recv( CSmtpResponse & clsResponse );

	/** SMTP 서버 도메인 or IP 주소 */
	std::string m_strServerIp;

	/** SMTP 서버 포트 번호 */
	int					m_iServerPort;

	/** SMTP 통신을 TLS 기반으로 하면 true 를 입력하고 그렇지 않으면 false 를 입력한다. */
	bool				m_bUseTls;

	/** SMTP 로그인 사용자 아이디 */
	std::string m_strUserId;

	/** SMTP 로그인 비밀번호 */
	std::string m_strPassWord;

	/** 발신자 이메일 주소 */
	std::string m_strEmailFrom;

	/** 수신자 이메일 주소 */
	std::string m_strEmailTo;

	/** 이메일 주제 */
	std::string m_strSubject;

	/** 이메일 내용 */
	std::string m_strContent;

	/** 이메일 첨부파일 */
	std::string m_strAttachFileName;

	Socket			m_hSocket;
	SSL					* m_psttSsl;
	int					m_iTimeout;
};

#endif
