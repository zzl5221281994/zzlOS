#ifndef tiny_string_H
#define tiny_string_H
#define       isDigts(     X                      )     (((X)>='0'&&(X)<='9')?1:0                         )
#define      isLetter(     X                      )     ((((X)>='a'&&(X)<='z')||((X)>='A'&&(X)<='Z'))?1:0 )
#define isLowerLetter(     X                      )     (((X)>='a'&&(X)<='z')?1:0                         )
#define isUpperLetter(     X                      )     (((X)>='A'&&(X)<='Z')?1:0                         )
extern void intToStr           (unsigned int num,char*desc,int bufSize					                  );
extern int tiny_strlen       (char*str                                                                    );
#endif