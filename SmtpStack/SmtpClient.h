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

enum ESmtpLang
{
	E_SL_EN = 0,
	E_SL_KO
};

/**
 * @defgroup SmtpStack SmtpStack
 * SMTP Ŭ���̾�Ʈ ���̺귯��
 */

/**
 * @ingroup SmtpStack
 * @brief SMTP Ŭ���̾�Ʈ - SMTP ������ ������ �����Ѵ�.
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
	bool SetLang( ESmtpLang eLang );

	void ClearEmail();

	bool Connect( );
	void Close( );

	bool Send( );

private:

	bool Send( std::string & strRequest, CSmtpResponse & clsResponse, int iWantCode = 0 );
	bool Send( const char * pszRequest, CSmtpResponse & clsResponse, int iWantCode = 0 );
	bool Send( std::string & strRequest );
	bool Recv( CSmtpResponse & clsResponse );

	bool AddBase64( const char * pszData, int iDataLen, std::string & strSendBuf );
	bool AddLangBuf( std::string & strInput, std::string & strSendBuf );

	/** SMTP ���� ������ or IP �ּ� */
	std::string m_strServerIp;

	/** SMTP ���� ��Ʈ ��ȣ */
	int					m_iServerPort;

	/** SMTP ����� TLS ������� �ϸ� true �� �Է��ϰ� �׷��� ������ false �� �Է��Ѵ�. */
	bool				m_bUseTls;

	/** SMTP �α��� ����� ���̵� */
	std::string m_strUserId;

	/** SMTP �α��� ��й�ȣ */
	std::string m_strPassWord;

	/** �߽��� �̸��� �ּ� */
	std::string m_strEmailFrom;

	/** ������ �̸��� �ּ� */
	std::string m_strEmailTo;

	/** �̸��� ���� */
	std::string m_strSubject;

	/** �̸��� ���� */
	std::string m_strContent;

	/** �̸��� ÷������ */
	std::string m_strAttachFileName;

	ESmtpLang		m_eLang;

	Socket			m_hSocket;
	SSL					* m_psttSsl;
	int					m_iTimeout;
};

#endif
