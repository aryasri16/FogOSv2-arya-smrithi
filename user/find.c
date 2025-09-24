#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"

char buf[1024];
int match(char*, char*);

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}
// ls implementation code
void
findit(char *path, char *expr)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  printf("Path: %s, expr: %s\n", path, expr);
  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_FILE) {
  	if (match(expr, fmtname(path))) {
  		printf("path: %s\n", path);
  	}
  } else if (T_DIR) {
  	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
  	  printf("find: path too long\n");
  	  close(fd);
  	  return;
  	}
  	strcpy(buf, path);
  	p = buf+strlen(buf);
  	*p++ = '/';
  	
  	    
  	while(read(fd, &de, sizeof(de)) == sizeof(de)){
  		if(de.inum == 0){
  	    	continue;
  	    }
		if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
			continue;
		}
  	    
  	    memmove(p, de.name, DIRSIZ);
  	    p[DIRSIZ] = 0;
  	    
  	    if(stat(buf, &st) < 0){
  	    	printf("find: cannot stat %s\n", buf);
  	        continue;
  	    }
		if(st.type == T_DIR){
			findit(buf, expr);
		} else if (match(expr,de.name)) {
			printf("path: %s\n", buf);
		}
  	}
  	close(fd);
  } 
}

int
main(int argc, char *argv[])
{
	  if(argc < 3){
	    fprintf(2, "usage: find path expression\n");
	    exit(1);
	  }
	  findit(argv[1], argv[2]);
  exit(0);
}
// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}
// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}
