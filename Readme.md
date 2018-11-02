# C++ Smtp stack 프로젝트
C++ STL 기반으로 SMTP 클라이언트 개발 프로젝트입니다.

### 개요
본 프로젝트의 목표는 다음과 같습니다.

* C++ STL 기반 SMTP 클라이언트 개발

### 개발자 정보
본 프로젝트를 진행하는 개발자 정보는 다음과 같습니다.

* 이메일 : websearch@naver.com
* 블로그 : http://blog.naver.com/websearch

### 라이선스

* 본 프로젝트의 라이선스는 GPLv3 이고 기업용 라이선스는 websearch@naver.com 으로 문의해 주세요.

### 폴더 설명
본 프로젝트에 포함된 폴더에 대한 설명은 다음과 같습니다.

* SipPlatform
  * C++ SIP stack 에서 사용되는 OS 독립적인 유틸리티 라이브러리

* SmtpStack
  * C++ STL 기반 SMTP 클라이언트

* TestSmtpStack
  * C++ Smtp stack 단위 테스트 및 테스트

### 컴파일 방법
본 프로젝트의 소스 코드를 컴파일하는 방법은 다음과 같습니다.

 * VC++ 2008
   - SipStack.sln 더블클릭한 후, 빌드한다.
   - Visual Studio 2008 버전 9.0.30729.1 SP 에서 빌드할 것을 권장합니다.

* 리눅스
  * make 를 실행한다.

### 소스 코드 예제
C++ SMTP 클라이언트로 이메일을 보내는 소스 코드 예제는 다음과 같습니다.

```
 CSmtpClient clsClient;
 bool bUseTls = false;
 
 // QQQ: pszServerIp, iServerPort 변수 선언 및 값 입력
 // QQQ: pszUserId, pszPassWord 변수 선언 및 값 입력
 // QQQ: pszEmailFrom, pszEmailTo 변수 선언 및 값 입력

 clsClient.SetServer( pszServerIp, iServerPort, bUseTls );
 clsClient.SetUser( pszUserId, pszPassWord );
 clsClient.SetFrom( pszEmailFrom );
 clsClient.SetTo( pszEmailTo );

 clsClient.SetSubject( "test" );
 clsClient.SetContent( "test email" );

 if( clsClient.Connect( ) == false )
 {
  printf( "connect SMTP server error\n" );
 }
 else if( clsClient.Send( ) == false )
 {
  printf( "send SMTP email error\n" );
 }
 ```
