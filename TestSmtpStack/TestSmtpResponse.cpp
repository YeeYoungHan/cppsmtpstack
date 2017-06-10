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
#include "SmtpResponse.h"
#include <stdarg.h>

bool TestSmtpResponse( const char * pszInput, int iCode, int iArgCount, ... )
{
	CSmtpResponse clsResponse;
	STRING_LIST::iterator	itSL;
	va_list pArgList;
	bool bError = false;
	int iLen;

	if( (iLen = clsResponse.Parse( pszInput, (int)strlen(pszInput) )) == -1 )
	{
		printf( "%s clsResponse.Parse(%s) error\n", __FUNCTION__, pszInput );
		return false;
	}

	if( iLen != (int)strlen(pszInput) )
	{
		printf( "%s iLen(%d) != input len(%d) (%s)\n", __FUNCTION__, iLen, (int)strlen(pszInput), pszInput );
		return false;
	}

	if( clsResponse.m_clsReplyList.size() != iArgCount )
	{
		printf( "%s m_clsReplyList.size(%d) != iArgCount(%d) (%s)\n", __FUNCTION__, (int)clsResponse.m_clsReplyList.size(), iArgCount, pszInput );
		return false;
	}

	va_start( pArgList, iArgCount );

	itSL = clsResponse.m_clsReplyList.begin();
	for( int i = 0; i < iArgCount; ++i )
	{
		char * pszArg = va_arg( pArgList, char * );

		if( strcmp( pszArg, itSL->c_str() ) )
		{
			printf( "%s [%d] input[%s] != reply[%s]\n", __FUNCTION__, i, pszArg, itSL->c_str() );
			bError = true;
		}
		
		++itSL;
	}

	va_end( pArgList );

	if( bError ) return false;

	return true;
}

bool TestSmtpResponse( )
{
	if( TestSmtpResponse( "220 ISIF.USC.EDU Service ready\r\n", 220, 1, "ISIF.USC.EDU Service ready" ) == false ) return false;

	if( TestSmtpResponse( "250-First line\r\n"
      "250-Second line\r\n"
      "250-234 Text beginning with numbers\r\n"
      "250 The last line\r\n"
			, 250
			, 4
			, "First line", "Second line", "234 Text beginning with numbers", "The last line" ) == false ) return false;

	printf( "%s OK!!!", __FUNCTION__ );
	
	return true;
}
