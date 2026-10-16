# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 언어 규칙

- 변수명과 함수명만 영어로 작성한다.
- 그 외 모든 것(주석, 커밋 메시지, 문서, 설명, 사용자 응답)은 한글로 작성한다.

## 프로젝트

C, HTML, Java로 만드는 개발자 웹 이력서. 단계별 계획은 @ROADMAP.md 참고.

- `gen/main.c`: `data/resume.txt`를 읽어 `public/`에 멀티 페이지 HTML(index, career, projects, contact) 생성 (C)
- `server/Server.java`: 생성된 HTML을 서빙하는 웹 서버 (Java, 외부 라이브러리 없이 `com.sun.net.httpserver` 사용)

## 빌드/실행

- 생성기 빌드: `gcc gen/main.c -o gen/gen`
- 서버 실행: `javac server/Server.java && java -cp server Server` (기본 포트 8080)
