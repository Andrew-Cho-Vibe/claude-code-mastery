# 개발자 웹 이력서

C, HTML, Java로 만든 개발자 웹 이력서입니다.

## 구성

- `data/resume.txt`: 이력 데이터 (`[섹션]` 헤더 + `key: value`)
- `gen/main.c`: 데이터를 읽어 `public/`에 `index/career/projects/contact.html` 4개 페이지를 생성하는 C 생성기
- `server/Server.java`: `public/`의 정적 파일(HTML, CSS, SVG 등)을 서빙하는 Java 웹 서버
- `public/style.css`, `public/img/`: 공통 스타일과 이미지 (사진은 `resume.txt`의 `photo`/`image` 경로로 교체)

## 빌드/실행

프로젝트 루트에서 실행합니다.

```
gcc gen/main.c -o gen/gen
./gen/gen
javac server/Server.java
java -cp server Server
```

브라우저에서 http://localhost:8080 을 엽니다. 포트는 첫 번째 인자로 바꿀 수 있습니다.

## 이력 수정

`data/resume.txt`를 고친 뒤 `./gen/gen`을 다시 실행하면 `public/*.html`이 갱신됩니다.
