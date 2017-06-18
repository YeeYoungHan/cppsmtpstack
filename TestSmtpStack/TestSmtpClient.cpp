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

#include "TestSmtpStack.h"
#include "SmtpClient.h"
#include "Log.h"

#define USE_KO

bool TestSmtpClient( int argc, char *argv[] )
{
	CSmtpClient clsClient;
	bool bUseTls = false;

	if( argc < 7 )
	{
		printf( "%s {server ip} {server port} {user id} {password} {from} {to}\n", argv[0] );
		return false;
	}

	if( argc == 8 )
	{
		bUseTls = true;
	}

	CLog::SetLevel( LOG_DEBUG | LOG_NETWORK );

	const char * pszServerIp = argv[1];
	int iServerPort = atoi(argv[2]);
	const char * pszUserId = argv[3];
	const char * pszPassWord = argv[4];
	const char * pszEmailFrom = argv[5];
	const char * pszEmailTo = argv[6];

	clsClient.SetServer( pszServerIp, iServerPort, bUseTls );
	clsClient.SetUser( pszUserId, pszPassWord );
	clsClient.SetFrom( pszEmailFrom );
	clsClient.SetTo( pszEmailTo );

#ifdef USE_KO
	clsClient.SetLang( E_SL_KO );
	clsClient.SetSubject( "테스트" );
	clsClient.SetContent( "테스트 이메일" );
#else
	clsClient.SetSubject( "test" );
	clsClient.SetContent( "test email" );
#endif

	//clsClient.SetAttachFile( "c:\\temp\\sqlite3.zip" );

	if( clsClient.Connect( ) == false )
	{
		printf( "connect SMTP server error\n" );
	}
	else if( clsClient.Send( ) == false )
	{
		printf( "send SMTP email error\n" );
	}

	return true;
}
