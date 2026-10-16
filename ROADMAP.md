# 개발자 웹 이력서 ROADMAP

C, HTML, Java로 만드는 간단한 개발자 웹 이력서.

## 목표

- 한 페이지로 핵심 이력을 보여주는 웹 이력서
- 이력 내용은 데이터 파일에 두고, 화면은 HTML로 생성
- 외부 프레임워크 없이 기본 기술만 사용

## 기술 스택과 역할

| 기술 | 역할 |
|------|------|
| HTML | 이력서 페이지 구조 (+ 기본 CSS 인라인/단일 파일) |
| C | `resume.txt`를 읽어 `index.html`을 생성하는 CLI 생성기 |
| Java | 생성된 HTML을 서빙하는 간단한 웹 서버 (`com.sun.net.httpserver`) |

## 이력서 내용 (일반 구성)

1. **기본 정보** — 이름, 직무, 이메일, GitHub
2. **자기소개** — 2~3문장
3. **기술 스택** — C, HTML, Java
4. **경력** — 회사명, 기간, 담당 업무
5. **프로젝트** — 이름, 설명, 사용 기술
6. **학력/자격증**

## 폴더 구조

```
resume/
├─ data/resume.txt     # 이력 데이터
├─ gen/main.c          # HTML 생성기 (C)
├─ server/Server.java  # 웹 서버 (Java)
└─ public/             # 생성 결과 (index/career/projects/contact.html) + style.css, img/
```

## Phase 1. 데이터와 HTML 초안

- [x] `data/resume.txt` 작성 (섹션별 `key: value` 형식)
- [x] `public/index.html` 수기 작성으로 레이아웃 확정
- [x] 기본 CSS 적용 (단일 컬럼, 모바일에서도 읽기 가능)

**완료 기준:** 브라우저에서 파일을 열면 이력서가 보임

## Phase 2. C 생성기

- [x] `resume.txt` 파싱 (한 줄씩 읽기)
- [x] 섹션별 HTML 출력 함수 작성
- [ ] `gcc gen/main.c -o gen/gen`으로 빌드
- [ ] 실행 결과를 `public/index.html`로 저장

**완료 기준:** 데이터 수정 후 생성기를 돌리면 HTML이 갱신됨

## Phase 3. Java 서버

- [x] `HttpServer`로 8080 포트 서버 작성
- [x] `/` 요청 시 `public/index.html` 응답
- [ ] `javac`/`java`로 실행

**완료 기준:** `http://localhost:8080`에서 이력서 확인

## Phase 4. 마무리

- [ ] 오타·링크 점검
- [x] README에 빌드/실행 방법 기록
- [ ] (선택) 정적 HTML만 GitHub Pages에 배포

## 일정

| Phase | 예상 기간 |
|-------|-----------|
| 1 | 0.5일 |
| 2 | 1일 |
| 3 | 0.5일 |
| 4 | 0.5일 |
| **합계** | **약 2~3일** |
