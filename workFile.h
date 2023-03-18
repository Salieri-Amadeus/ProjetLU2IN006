typedef struct {
    char * name ;
    char * hash ;
    int mode ;
} WorkFile ;

typedef struct {
    WorkFile * tab ;
    int size ;
    int n ;
 } WorkTree ;
