#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 64
#define MAX_PAIRS 16
#define KEY_LEN 32
#define VAL_LEN 512

typedef struct {
    char name[KEY_LEN];
    int count;
    char keys[MAX_PAIRS][KEY_LEN];
    char vals[MAX_PAIRS][VAL_LEN];
} Block;

typedef struct {
    const char *file;
    const char *label;
} NavItem;

static const NavItem nav_items[] = {
    {"index", "홈"},
    {"career", "경력"},
    {"projects", "프로젝트"},
    {"contact", "연락처"},
};

static Block blocks[MAX_BLOCKS];
static int block_count = 0;

static void trim(char *s) {
    char *start = s;
    while (*start == ' ' || *start == '\t') start++;
    size_t len = strlen(start);
    while (len > 0 && (start[len - 1] == '\n' || start[len - 1] == '\r' || start[len - 1] == ' ' || start[len - 1] == '\t')) len--;
    memmove(s, start, len);
    s[len] = '\0';
}

static int parse_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;
    char line[VAL_LEN + KEY_LEN + 8];
    Block *cur = NULL;
    while (fgets(line, sizeof line, fp)) {
        trim(line);
        if (line[0] == '\0' || line[0] == '#') continue;
        if (line[0] == '[') {
            char *end = strchr(line, ']');
            if (!end || block_count >= MAX_BLOCKS) continue;
            *end = '\0';
            cur = &blocks[block_count++];
            snprintf(cur->name, KEY_LEN, "%s", line + 1);
        } else if (cur && cur->count < MAX_PAIRS) {
            char *colon = strchr(line, ':');
            if (!colon) continue;
            *colon = '\0';
            char *val = colon + 1;
            trim(line);
            trim(val);
            snprintf(cur->keys[cur->count], KEY_LEN, "%s", line);
            snprintf(cur->vals[cur->count], VAL_LEN, "%s", val);
            cur->count++;
        }
    }
    fclose(fp);
    return 1;
}

static const char *get(const Block *b, const char *key) {
    for (int i = 0; i < b->count; i++)
        if (strcmp(b->keys[i], key) == 0) return b->vals[i];
    return "";
}

static const Block *find(const char *name) {
    for (int i = 0; i < block_count; i++)
        if (strcmp(blocks[i].name, name) == 0) return &blocks[i];
    return NULL;
}

static void esc(FILE *out, const char *s) {
    for (; *s; s++) {
        switch (*s) {
            case '&': fputs("&amp;", out); break;
            case '<': fputs("&lt;", out); break;
            case '>': fputs("&gt;", out); break;
            case '"': fputs("&quot;", out); break;
            default: fputc(*s, out);
        }
    }
}

static void print_tags(FILE *out, const char *list, const char *tag_class) {
    char buf[VAL_LEN];
    snprintf(buf, sizeof buf, "%s", list);
    fprintf(out, "<div class=\"tags %s\">", tag_class);
    for (char *tok = strtok(buf, ","); tok; tok = strtok(NULL, ",")) {
        trim(tok);
        if (tok[0] == '\0') continue;
        fputs("<span class=\"tag\">", out);
        esc(out, tok);
        fputs("</span>", out);
    }
    fputs("</div>", out);
}

static void print_link(FILE *out, const char *cls, const char *href, const char *label, int external) {
    fprintf(out, "<a class=\"%s\" href=\"", cls);
    esc(out, href);
    fputs(external ? "\" target=\"_blank\" rel=\"noopener\">" : "\">", out);
    esc(out, label);
    fputs("</a>", out);
}

static void print_img(FILE *out, const char *cls, const char *src, const char *alt) {
    if (src[0] == '\0') return;
    fprintf(out, "<img class=\"%s\" src=\"", cls);
    esc(out, src);
    fputs("\" alt=\"", out);
    esc(out, alt);
    fputs("\">\n", out);
}

static void print_head(FILE *out, const char *page_title, const char *active) {
    const Block *profile = find("profile");
    const char *name = profile ? get(profile, "name") : "";
    fputs("<!DOCTYPE html>\n<html lang=\"ko\">\n<head>\n<meta charset=\"UTF-8\">\n"
          "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<title>", out);
    esc(out, page_title);
    fputs(" - ", out);
    esc(out, name);
    fputs("</title>\n<link rel=\"stylesheet\" href=\"style.css\">\n</head>\n<body>\n"
          "<nav class=\"nav\"><div class=\"nav-inner\">\n<a class=\"brand\" href=\"index.html\">", out);
    esc(out, name);
    fputs("</a>\n<ul>\n", out);
    for (size_t i = 0; i < sizeof nav_items / sizeof nav_items[0]; i++) {
        int is_active = strcmp(nav_items[i].file, active) == 0;
        fprintf(out, "<li><a href=\"%s.html\"%s>%s</a></li>\n",
                nav_items[i].file, is_active ? " class=\"active\"" : "", nav_items[i].label);
    }
    fputs("</ul>\n</div></nav>\n<main>\n", out);
}

static void print_foot(FILE *out) {
    const Block *profile = find("profile");
    fputs("</main>\n<footer class=\"footer\">© ", out);
    esc(out, profile ? get(profile, "name") : "");
    fputs("</footer>\n</body>\n</html>\n", out);
}

static FILE *open_page(const char *dir, const char *file) {
    char path[512];
    snprintf(path, sizeof path, "%s/%s.html", dir, file);
    FILE *out = fopen(path, "w");
    if (!out) fprintf(stderr, "출력 파일을 열 수 없습니다: %s\n", path);
    return out;
}

static void write_home(FILE *out) {
    const Block *p = find("profile");
    const Block *sns = find("sns");
    const Block *about = find("about");
    const Block *skills = find("skills");

    print_head(out, "홈", "index");
    fputs("<section class=\"hero\">\n", out);
    if (p) print_img(out, "avatar", get(p, "photo"), get(p, "name"));
    fputs("<div>\n<h1>", out);
    if (p) esc(out, get(p, "name"));
    fputs("</h1>\n<p class=\"role\">", out);
    if (p) esc(out, get(p, "role"));
    fputs("</p>\n<p class=\"tagline\">", out);
    if (p) esc(out, get(p, "tagline"));
    fputs("</p>\n<div class=\"btns\">", out);
    print_link(out, "btn primary", "projects.html", "프로젝트 보기", 0);
    if (p) {
        char mail[VAL_LEN + 8];
        snprintf(mail, sizeof mail, "mailto:%s", get(p, "email"));
        print_link(out, "btn", mail, "이메일", 0);
        print_link(out, "btn", get(p, "github"), "GitHub", 1);
    }
    if (sns)
        for (int i = 0; i < sns->count; i++)
            print_link(out, "btn", sns->vals[i], sns->keys[i], 1);
    fputs("</div>\n</div>\n</section>\n", out);

    if (about) {
        fputs("<section class=\"section\">\n<h2>자기소개</h2>\n<div class=\"panel\"><p>", out);
        esc(out, get(about, "text"));
        fputs("</p></div>\n</section>\n", out);
    }
    if (skills) {
        fputs("<section class=\"section\">\n<h2>기술 스택</h2>\n", out);
        print_tags(out, get(skills, "list"), "skills");
        fputs("\n</section>\n", out);
    }
    print_foot(out);
}

static void write_career(FILE *out) {
    print_head(out, "경력", "career");
    fputs("<h1 class=\"page-title\">경력</h1>\n<p class=\"page-lead\">걸어온 길과 학력·자격 사항입니다.</p>\n"
          "<ol class=\"timeline\">\n", out);
    for (int i = 0; i < block_count; i++) {
        if (strcmp(blocks[i].name, "career") != 0) continue;
        fputs("<li><span class=\"period\">", out);
        esc(out, get(&blocks[i], "period"));
        fputs("</span>\n<h3>", out);
        esc(out, get(&blocks[i], "company"));
        fputs("</h3>\n<p class=\"desc\">", out);
        esc(out, get(&blocks[i], "duty"));
        fputs("</p></li>\n", out);
    }
    fputs("</ol>\n", out);

    const Block *edu = find("education");
    if (edu) {
        fputs("<section class=\"section\">\n<h2>학력/자격증</h2>\n<div class=\"panel\"><ul class=\"edu\">\n", out);
        for (int i = 0; i < edu->count; i++) {
            fputs("<li>", out);
            esc(out, edu->vals[i]);
            fputs("</li>\n", out);
        }
        fputs("</ul></div>\n</section>\n", out);
    }
    print_foot(out);
}

static void write_projects(FILE *out) {
    print_head(out, "프로젝트", "projects");
    fputs("<h1 class=\"page-title\">프로젝트</h1>\n<p class=\"page-lead\">직접 만든 프로젝트 목록입니다.</p>\n"
          "<div class=\"grid\">\n", out);
    for (int i = 0; i < block_count; i++) {
        if (strcmp(blocks[i].name, "project") != 0) continue;
        fputs("<article class=\"card\">\n", out);
        print_img(out, "thumb", get(&blocks[i], "image"), get(&blocks[i], "name"));
        fputs("<div class=\"card-body\">\n<h3>", out);
        esc(out, get(&blocks[i], "name"));
        fputs("</h3>\n<p class=\"desc\">", out);
        esc(out, get(&blocks[i], "desc"));
        fputs("</p>\n", out);
        print_tags(out, get(&blocks[i], "tech"), "");
        fputs("\n</div>\n</article>\n", out);
    }
    fputs("</div>\n", out);
    print_foot(out);
}

static void write_contact(FILE *out) {
    const Block *p = find("profile");
    const Block *sns = find("sns");

    print_head(out, "연락처", "contact");
    fputs("<h1 class=\"page-title\">연락처</h1>\n<p class=\"page-lead\">편하게 연락 주세요.</p>\n<div>\n", out);
    if (p) {
        fputs("<a class=\"contact-card\" href=\"mailto:", out);
        esc(out, get(p, "email"));
        fputs("\"><span>이메일</span><span>", out);
        esc(out, get(p, "email"));
        fputs("</span></a>\n<a class=\"contact-card\" href=\"", out);
        esc(out, get(p, "github"));
        fputs("\" target=\"_blank\" rel=\"noopener\"><span>GitHub</span><span>", out);
        esc(out, get(p, "github"));
        fputs("</span></a>\n", out);
    }
    if (sns) {
        for (int i = 0; i < sns->count; i++) {
            fputs("<a class=\"contact-card\" href=\"", out);
            esc(out, sns->vals[i]);
            fputs("\" target=\"_blank\" rel=\"noopener\"><span>", out);
            esc(out, sns->keys[i]);
            fputs("</span><span>", out);
            esc(out, sns->vals[i]);
            fputs("</span></a>\n", out);
        }
    }
    fputs("</div>\n", out);
    print_foot(out);
}

int main(int argc, char *argv[]) {
    const char *in_path = argc > 1 ? argv[1] : "data/resume.txt";
    const char *out_dir = argc > 2 ? argv[2] : "public";

    if (!parse_file(in_path)) {
        fprintf(stderr, "입력 파일을 열 수 없습니다: %s\n", in_path);
        return 1;
    }

    struct {
        const char *file;
        void (*write)(FILE *);
    } pages[] = {
        {"index", write_home},
        {"career", write_career},
        {"projects", write_projects},
        {"contact", write_contact},
    };
    for (size_t i = 0; i < sizeof pages / sizeof pages[0]; i++) {
        FILE *out = open_page(out_dir, pages[i].file);
        if (!out) return 1;
        pages[i].write(out);
        fclose(out);
    }
    return 0;
}
