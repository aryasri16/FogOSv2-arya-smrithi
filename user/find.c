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
  	    //printf("%d %d %d %d\n", fmtname(buf), st.type, st.ino, (int) st.size);
		if(st.type == T_DIR){
			findit(buf, expr);
		} else if (match(expr,de.name)) {
			printf("path: %s\n", buf);
		}
  	}
  	close(fd);
  } /*else {
  	printf("Error found while traversing dirs \n");
  }*/
  /*
  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    printf("%d %d %d %d\n", fmtname(path), st.type, st.ino, (int) st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    
    - recursion would happen when we're searching within directories to find the file name given in expr
    - stringcmp(TFILE == expr), else 
    - base case would be the tree for all files
    
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      printf("%d %d %d %d\n", fmtname(buf), st.type, st.ino, (int) st.size);
    }
    break;
  }
  close(fd); */
}
/*
void
findit(char *path, char *expression)
{
  int n, m;
  char *p, *q;
  int fd = 0;
  printf("Expression: %s\n", expression);
  printf("Path: %s \n", path);

  m = 0;
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
    m += n;
    buf[m] = '\0';
    p = buf;
    while((q = strchr(p, '\n')) != 0){
      *q = 0;
      if(match(path, p)){
        *q = '\n';
        write(1, p, q+1 - p);
      }
      p = q+1;
    }
    if(m > 0){
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}*/
int
main(int argc, char *argv[])
{
	  if(argc < 3){
	    fprintf(2, "usage: find path expression\n");
	    exit(1);
	  }
	  findit(argv[1], argv[2]);
/*
  int fd, i;
  char *path;
  char *expression;

  printf("in user space find.c file\n");

  if(argc <= 1){
    fprintf(2, "usage: find pattern [file ...]\n");
    exit(1);
  }
  path = argv[1];
  expression = argv[2];

  if(argc <= 2){
    findit(path, expression);
    printf("find: unable to find due to lack of arguments");
    exit(0);
  }

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("find: cannot open %s\n", argv[i]);
      exit(1);
    }
    findit(path, expression);
    close(fd);
  }
  */
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
