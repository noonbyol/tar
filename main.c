#include <fcntl.h>
#include "header.h"

typedef struct Listnode
{
    char*name;
    struct Listnode*next;
} listnode;

typedef struct s_posix_header
{                             
    char name[100];               
    char mode[8];                
    char uid[8];                  
    char gid[8];                  
    char size[12];                
    char mtime[12];               
    char chksum[8];               
    char typeflag;               
    char linkname[100];          
    char magic[6];                
    char version[2];              
    char uname[32];               
    char gname[32];               
    char devmajor[8];             
    char devminor[8];             
    char prefix[155];             
    char new_arr[12];            
                                  
} posix_header;

void add_elem(listnode** list, char * string){
    listnode* temp = (listnode*)malloc(sizeof(listnode));
    temp->name = malloc(_strlen(string) +  1);
    _strcpy(temp->name, string);
    temp->next = NULL;
    if(*list == NULL)
        *list = temp;
    else{
        listnode* current = *list;
        while(current->next)
            current = current->next;
        current->next = temp;
    }
}

int oct_dec(char* s){
    int leng = _strlen(s);
    int num = 0, bas = 1;

    for (int i = leng - 1; i >= 0; i--) {
        int n = s[i] - '0';
        num += n * bas;
        bas *= 8;
    }
    return num;
}

char*dec_oct(long n, int size)
{
   char *oct_num = (char*)malloc(size*sizeof(char));
    making_null(oct_num, size);
    int i = 0;
    while (n != 0) 
    {
        oct_num[i] = n % 8 + '0';
        n = n / 8;
        i++;
    }
    int left = 0, right = i - 1;
    while(left < right)
    {
        char c = oct_num[left];
        oct_num[left] = oct_num[right];
        oct_num[right] = c;
        left++;
        right--;
    }
    return oct_num;
}

void posix_null(posix_header *p)
{
    making_null(p->name, 100);
    making_null(p->mode, 8);
    making_null(p->uid, 8);
    making_null(p->gid, 8);
    making_null(p->size, 12);
    making_null(p->mtime, 12);
    making_null(p->chksum, 8);
    making_null(p->linkname, 100);
    making_null(p->magic, 6);
    making_null(p->version, 2);
    making_null(p->uname, 32);
    making_null(p->gname, 32);
    making_null(p->devmajor, 8);
    making_null(p->devminor, 8);
    making_null(p->prefix, 155);
    making_null(p->new_arr, 12);
}

void pos_content(posix_header *p, char*s)
{
    struct stat fs;
    char*d = "docode";
    stat(s, &fs);
    posix_null(p);
    int sum = 256;
    _strcpy(p->name, s);
    _strcpy(p->mode, dec_oct(fs.st_mode, 8));
    _strcpy(p->uid, dec_oct(fs.st_uid, 8));
    _strcpy(p->gid, dec_oct(fs.st_gid, 8));
    _strcpy(p->size, dec_oct(fs.st_size, 12));
    _strcpy(p->mtime, dec_oct(fs.st_mtim.tv_sec, 12));
    _strcpy(p->magic, "ustar  ");
    _strcpy(p->uname, d);
    _strcpy(p->gname, d);
    p->typeflag = '0';
    sum += sum_of_ascii(p->name);
    sum += sum_of_ascii(p->mode);
    sum += sum_of_ascii(p->uid);
    sum += sum_of_ascii(p->gid);
    sum += sum_of_ascii(p->size);
    sum += sum_of_ascii(p->mtime);
    sum += (int)'0';
    sum += sum_of_ascii(p->magic);
    sum += sum_of_ascii(p->uname);
    sum += sum_of_ascii(p->gname);
    _strcpy(p->chksum, dec_oct(sum, 8));
}


void  read_save(int fd, char*name)
{
        posix_header pos;   
        pos_content(&pos, name);
        write(fd, &pos, 512);

        char*ans = (char*)malloc(512*sizeof(char));
        making_null(ans, 513);

        int fd2 = open(pos.name, O_RDONLY);
        err_no(fd2, pos.name);
        while(read(fd2, ans, 512) > 0)
        {
            write(fd, ans, 512);
            making_null(ans, 512);
        }
        free(ans);
        close(fd2);
}

void c_opt(int ac, char**av)
{
    int fd = open(av[2], O_CREAT | O_WRONLY, 0644);
    listnode *list = NULL;
    for(int i = 3; i < ac; i++)
        add_elem(&list, av[i]);
    listnode * n2 = list;
    while(n2)
    {
        char*name = n2->name;
        read_save(fd, name);
        n2 = n2->next;
    }
    close(fd);
}

void r_opt(int ac, char**av)
{
    int fd = open(av[2], O_WRONLY | O_APPEND);
    err_no(fd,av[2]);
    for(int i = 3; i < ac; i++)
        read_save(fd, av[i]);
    
}

void t_opt(int ac, char**av)
{
    int fd = open(av[2], O_RDONLY);
    err_no(fd, av[2]);
    posix_header pos;
    posix_null(&pos);
    while(read(fd, &pos, 512)>0)
    {
        if(!_strcmp(pos.magic, "ustar  "))
        {
            int len = _strlen(pos.name);
            write(1, pos.name, len);
            write(1,"\n", 1);
        }
    }
    close(fd);    
}

void u_opt(int ac, char**av)
{
    int fd = open(av[2], O_RDONLY);
    struct stat fs;
    posix_header pos;
    stat(av[3], &fs);

    int mod_av = (int)fs.st_mtim.tv_sec;
    int latest = mod_av;
    int flag = 0;

    while (read(fd, &pos, 512) > 0)
    {
        int new_time = oct_dec(pos.mtime);
        if (new_time > latest)
            latest = new_time;
    }
    close(fd);

    if (mod_av >= latest)
    {
        int fd2 = open(av[2], O_WRONLY | O_APPEND);
        err_no(fd2, av[2]);
        read_save(fd2, av[3]);
        close(fd2);
    }
}



void x_opt(int ac, char**av)
{
   int fd = open(av[2], O_RDONLY);
   err_no(fd, av[2]);
    posix_header pos;
    while(read(fd, &pos, 512))
    {  
    int len = _strlen(pos.name);
    int mode = oct_dec(pos.mode);
    int size = oct_dec(pos.size);
    char*file = (char*)malloc((len+1)*sizeof(char));
    making_null(file, len + 1);
    char*content = (char*)malloc(size*sizeof(char));
    making_null(content, size);
    
        if(!_strcmp(pos.magic, "ustar  "))
        {    
            _strcpy(file, pos.name);
            int leng = size;
            for(; leng > 0 ; leng -= 512)
            {
                char*temp = (char*)malloc(512*sizeof(char));
                read(fd, temp, 512);
                _strcat(content, temp);
                free(temp);
            }
            int fd2 = open(file, O_CREAT | O_WRONLY, mode);
            write(fd2, content, size);
            close(fd2);
        }
        free(file);
        free(content);
    }
    close(fd);
}

int main(int argc, char**argv)
{
    int c = 0;
        for(int i = 1; i < argc; i++)
        {
            if(argc > 3)
            {
                if(_strcmp(argv[i], "-cf") == 0)
                    c_opt(argc, argv);
                if(_strcmp(argv[i], "-rf") == 0)
                    r_opt(argc, argv);
                if(_strcmp(argv[i], "-uf") == 0)
                    u_opt(argc, argv);
                          
            }
            if(_strcmp(argv[i], "-tf") == 0)
                t_opt(argc, argv);
            if(_strcmp(argv[i], "-xf") == 0)
                x_opt(argc, argv);     
        }
    return 0;
}