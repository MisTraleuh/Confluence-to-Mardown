#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

typedef struct md_s {
    char *path_and_file;
    struct md_s *next;
} md_t;

char *add_two_string(char *str1, char *str2);

void destroy_list(md_t **all_md)
{
    md_t *tmp = (*all_md);
    while (tmp)
    {
        md_t *tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2->path_and_file);
        free(tmp2);
    }
    (*all_md) = NULL;
}

void push_node_at_back(char *filepath, md_t **head)
{
    md_t *new_node = malloc(sizeof(md_t));

    new_node->path_and_file = strdup(filepath);
    new_node->next = NULL;
    if ((*head) == NULL) {
        (*head) = new_node;
        return;
    }
    md_t *tmp = (*head);
    for (; tmp->next != NULL; tmp = tmp->next);
    tmp->next = new_node;
}

int count_nodes(md_t *head)
{
    int count = 0;
    md_t *tmp = head;

    for (; tmp != NULL; tmp = tmp->next)
        count++;
    return count;
}

void print_linked_list(md_t *head)
{
    md_t *tmp = head;
    for (; tmp != NULL; tmp = tmp->next)
        printf("%s\n", tmp->path_and_file);
}

int endwith(char *src, char *end)
{
    int size = strlen(src) - strlen(end);

    if (size < 0)
        return 1;
    if (!strcmp(src + size, end))
        return 0;
    return 1;
}

void read_tree(char *basePath, int root, md_t **head, char *extension)
{
    char path[1000] = {0};
    struct dirent *dp = NULL;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
            if (dp->d_type == DT_REG && !endwith(dp->d_name, extension))
                push_node_at_back(add_two_string(basePath, add_two_string("/", dp->d_name)), head);
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            read_tree(path, root + 2, head, extension);
        }
    }
    closedir(dir);
}

char *add_two_string(char *str1, char *str2)
{
    int i = 0;
    int y = 0;
    // create a new string with the size of the two strings
    char *res = malloc(strlen(str1) + strlen(str2) + 1);
    // loop through the first string and add it to the new string
    for (; str1[i]; i++)
        res[i] = str1[i];
    // loop through the second string and add it to the new string
    for (; str2[y]; y++)
        res[i + y] = str2[y];
    // add the null character to the end of the new string
    res[i + y] = '\0';
    return res;
}

void message_error(char *message, ...)
{
    char *str = NULL;
    // like this with a infinity arguments -> printf("%s", message);
    va_list args;
    // start the ... -> infinity arguments
    va_start(args, message);
    // format the message -> %s
    vfprintf(stderr, message, args);
    // free the ... -> infinity arguments
    va_end(args);
    // exit the program with error code 1
    exit(1);
}

int isDirectory(const char *path)
{
    // Create a stat struct to get the file type
    struct stat statbuf;
    // if stat returns 0, then the file not exists
    if (stat(path, &statbuf) != 0)
        return 0;
    // else if the file is a directory, return 1
    return S_ISDIR(statbuf.st_mode);
}

void check_error(int argc, char **argv)
{
    // check if the user enter the right number of arguments
    if (argc != 3)
        message_error("\e[31mUsage: %s <folder> <result>\e[0m\n", argv[0]);
    // add the help flag for the user
    if (!strcmp("-h", argv[1]))
        message_error("\e[31mUsage: %s <folder> <result>\e[0m\n", argv[0]);
    // check if the folder exist
    if (!isDirectory(argv[1]))
        message_error("\e[31m%s is not a directory\e[0m\n", argv[1]);
    if (isDirectory(argv[2]))
        message_error("\e[31m%s exist, please enter a folder who doesn't exist\e[0m\n", argv[2]);
}

int extension(char *str)
{
    int i = 0;
    // go to the end of the string
    for (; str[i]; i++);
    // check if the last character is a .html and return 1 if it is
    if (str[--i] == 'l' && str[--i] == 'm' && str[--i] == 't' && str[--i] == 'h' && str[--i] == '.')
        return 1;
    return 0;
}

/* 
*/ 
FILE *read_file(char *filepath, char *path)
{
    // open the file in read mode and on a FILE *
    FILE *file = fopen(add_two_string(path, filepath), "r");
    // if the file doesn't have the right permission, return NULL
    if (!file)
        message_error("%s: %s\n", path, strerror(errno));
    return file;
}

char *add_slash(char *path)
{
    int i = 0;
    char *res = NULL;
    // go to the end of the string
    for (; path[i]; i++);
    // if the last character is not a /, add it on a other string (yes I know it's ugly but is c )
    if (path[i - 1] != '/') {
        // alloc the memory for the new string
        res = malloc(i + 2);
        // add the end of the string
        res[i + 1] = '\0';
        // copy the string
        for (i = 0; path[i]; i++)
            res[i] = path[i];
        // add the slash
        res[i] = '/';
        return res;
    }
    return path;
}
/*
    Like strcmp() but compare two strings at different index for str1.
    Is also index is for str2 not str1. By default str1 index is 0.
    For you double index use compare_string_position(str1, str2, index_str1, index_str2)

    Exemplary use:
    compare_string_position("hello", "salut je suis ! hello", 15) -> 1
    compare_string_position("hello", "salut je suis ! hello", 0) -> 0
*/
int compare_string_index(char *str, char *str2, int index)
{
    // compare the string at the index with the string2
    for (int i = 0; str[i]; i++)
        if (str[i] != str2[i + index])
            return 0;
    return 1;
}

/*
    Like strncmp but compare the string at the index with the string2
    exemple :
        +---------------+
        |str = "hello"  |
        |str2 = "hello" |      return 1
        |index = 1      |  (they are equal)
        |position = 3   |
        +---------------+
        +---------------+
        |str = "heAlo"  |
        |str2 = "hello" |      return 0
        |index = 1      |  (they aren't equal)
        |position = 0   |
        +---------------+
*/
int compare_string_position(char *str, char *str2, int position, int index)
{
    // compare the string at the position with the string2
    for (int i = 0; i != position; i++) {
        if (str[i] != str2[i + index])
            return 0;
    }
    return 1;
}

/*
    This function if used by parse the name of a folder
    exemple : "<a href="17281984.html">R&amp;S®Web Application Firewall 6.5.6 (EN)</a>" -> return "R&amp;S®Web Application Firewall 6.5.6 (EN)"
    On a new string with the size of the string + the size of the string2 + 1
    Don't forget to free the new string
*/
char *parse_name(char *str)
{
    int i = 0;
    int a = 0;
    // alloc the memory for the new string
    char *res = malloc(strlen(str));

    // On confluence a line of the file is like this :
    // <span><a href="17281984.html">R&amp;S®Web Application Firewall 6.5.6 (EN)</a></span>
    // For take the name of the file,we go at the first '/'
    for (; str[i] != '/'; i++);
    if (str[i + 1] != 'a')
        for (; str[i] != '/'; i++);
    // now we back off at the first > 
    for (; str[i] != '>'; i--);
    i++;
    // now copy the name of the file
    for (; str[i] != '<'; i++, a++)
        res[a] = str[i] == '/' ? '_' : str[i];
    // add the null character to the end of the new string
    res[a] = '\0';
    return res;
}

/*
    This function is used for add two string with a caractere and this end of the string
    exemple : add_two_string_end("hello", "world", '!') -> return "hello world!"
*/
char *add_two_string_char(char *str1, char *str2, char c)
{
    int i = 0;
    int y = 0;
    // create a new string with the size of the two strings
    char *res = malloc(strlen(str1) + strlen(str2) + 2);
    // copy the first string
    for (; str1[i]; i++)
        res[i] = str1[i];
    // copy the second string
    for (; str2[y]; y++)
        res[i + y] = str2[y];
    // add the character c at the end of the new string
    res[i + y] = c;
    // add the null character to the end of the new string
    res[i + y + 1] = '\0';
    return res;
}

char *parse_folder(char *folder, char *str)
{
    return add_two_string_char(folder, parse_name(str), '/');;
}

/* take a string and return the string like this
    exemple : replace_space("<a href="17281984.html">R&amp;S®Web Application Firewall 6.5.6 (EN)</a>")
    return "<a\ href="17281984.html">R&amp;S®Web\ Application\ Firewall\ 6.5.6\ (EN)</a>"

    /!\ On a new string, the memory is allocated, so you have to free it /!\
*/
char *replace_space(char *str)
{
    int i = 0;
    int a = 0;
    // alloc the memory for the new string
    char *res = malloc(strlen(str) * 2);
    // loop for replace all caracters by '\ "
    for (; str[i]; i++) {   
        if (str[i] != ' ' && str[i] != ')' && str[i] != '(' && str[i] != '&' && str[i] != ';')
            res[a++] = str[i];
        else {
            res[a++] = '\\';
            res[a++] = str[i];
        }
    }
    // add the null character to the end of the new string
    res[a] = '\0';
    return res;
}

char *create_command(char *folder, char *str)
{
    // alloc the memory for the new string
    char *mv = malloc(strlen(str) + strlen(folder) + 10);
    // add caracters to the new string
    mv[0] = 'c';
    mv[1] = 'p';
    mv[2] = ' ';
    // add on the new string the folder and the file
    for (int i = 0; str[i]; i++)
        mv[i + 3] = str[i];
    // add a space to the command
    mv[strlen(str) + 3] = ' ';
    // copy the folder
    for (int i = 0; folder[i]; i++)
        mv[i + strlen(str) + 4] = folder[i];
    // add the null character to the end of the new string
    mv[strlen(str) + strlen(folder) + 4] = '\0';
    return mv;
}
// take a filename and return the id of the file name
char *find_number_of_folder(char *file_to_moove)
{
    int i = 0;
    int a = 0;
    // alloc the memory for the new string
    char *res = malloc(strlen(file_to_moove));

    // go to the end of the string
    for (; file_to_moove[i]; i++);
    // back off at the first '_' or '/'
    for (; i >= 0 && file_to_moove[i] != '_' && file_to_moove[i] != '/'; i--);
    // if the file is in the root folder don't do anything | else skip the first '_' or '/'
    if (i >= 0 && (file_to_moove[i] == '_' || file_to_moove[i] == '/'))
        i++;
    // copy the id of the file
    for (; file_to_moove[i] && file_to_moove[i] != '.'; i++, a++)
        res[a] = file_to_moove[i];
    // add the null character to the end of the new string
    res[a] = '\0';
    return res;
}

void moove_all_assets_on_folder(char *folder, char *path_and_file)
{
    // As said before, we need to check if the id of the end of file correxponds to the id of a folder
    // if the folder is not found, we moove nothing
    if (!isDirectory(path_and_file))
        return;
    // create a DIR * struct for read the directory with opendir()
    DIR *dir = opendir(path_and_file);
    // ceate a struct dirent * struct for read the directory with readdir()
    struct dirent *entry;
    // loop for read all the files in the directory
    while((entry = readdir(dir)) != NULL) {
        // we want all file and not the . and .. is faster than this
        // if (strcmp(entry->d_name, ".") || strcmp(entry->d_name, ".."))
        if (entry->d_name[0] != '.') {
            /*
                This line is very optimized, we don't need to create a new string for the new path
                I'm gonna to split this line like this :
                +-----------------------------------------+
                |system(                                  | call system() to execute the command in a new process to not block the program like a new terminal
                |    create_command(                      | create the command to execute
                |       replace_space(folder),            | replace all space by '\ ' | on a string like this : "hello world" => "hello\ world"
                |       replace_space(                    | Same up
                |           add_two_string(               | add the two string | on a string like this : "hello" + "world" => "helloworld"
                |               add_slash(path_and_file), | add the slash at the end of the string if this doesn't exist | on a string like this : "hello" => "hello/"
                |               entry->d_name             | The name of the file
                |           )                             |
                |       )                                 | Now system() will execute the command "cp <folder/id/file> <where this file is>"
                |   )                                     |
                |);                                       |
                +-----------------------------------------+
            */
            system(create_command(replace_space(folder), replace_space(add_two_string(add_slash(path_and_file), entry->d_name))));
        }
    }
}

void parse_file(FILE *file, char *path, char *file_to_moove, char *path_argv_1)
{
    char *str = NULL;
    size_t len = 0;
    int in_moove = 0;
    int i = 0;
    char *folder = add_slash(path);

    // read the file line by line
    while (getline(&str, &len, file) != -1) {
        // pass space and tab
        for (i = 0; str[i] == ' ' || str[i] == '\t'; i++);
        // in_moove is 1 if the line is a directory for build the tree
        if (in_moove == 1) {
            // if the line is </ol>, we stop all moove of file and create directory
            if (compare_string_index("</ol>", str, i)) {
                // execute on a system command to moove the file at this good place
                system(create_command(replace_space(folder), replace_space(file_to_moove)));
                // on confluence all file are in the same folder, and for each file a id at the at the end of the name is done like this :
                // Add-and-configure-a-Workflow-with-parameters_17282369.html
                // here 17282369 is a id, and this if correspond to the id of a folder (if this folder exist) of all assets of this file
                // we go tho split the id with the function find_number_of_folder(), this function return the id of the folder
                // we concat the id with the name of the directory like this : argv[1]/attachements/id | or here path_argv_1/attachements/id
                // the function move_all_assets_on_folder() moove all assets of the file at the good place (placement of the file)
                moove_all_assets_on_folder(folder, add_two_string(path_argv_1, find_number_of_folder(file_to_moove)));
                return;
            }
            // on confluence all directory start with <span>, and we compare this with the line 
            if (compare_string_position("<span>", str, 5, i)) {
                // parse the namre of the directory
                folder = parse_folder(folder, str);
                // create the directory
                mkdir(folder, 0777);
            }
        }
        // if the line is <ol id="breadcrumbs"> this line for confluence is all names for directory, and set in_moove at 1
        if (compare_string_index("<ol id=\"breadcrumbs\">", str, i)) {
            in_moove = 1;
        }
    }
}

char *replace_extension(char *str)
{
    char *str2 = strdup(str);
    int i = strlen(str);

    for (; str[i] != '/' && str2[i] != '_'; i--);
    if (str[i] == '/') {
        str2[++i] = 'i';
        str2[++i] = 'n';
        str2[++i] = 'd';
        str2[++i] = 'e';
        str2[++i] = 'x';
        str2[++i] = '.';
        str2[++i] = 'm';
        str2[++i] = 'd';
        str2[++i] = '\0';
    } else {
        str2[i] = '.';
        str2[++i] = 'm';
        str2[++i] = 'd';
        str2[++i] = '\0';
    }
    return str2;
}

void convert_html_to_md(md_t *all_md)
{
    md_t *tmp = all_md;

    for (; tmp; tmp = tmp->next)
        if (system(add_two_string("node index.js ", add_two_string(replace_space(tmp->path_and_file), add_two_string(">", replace_extension(replace_space(tmp->path_and_file)))))))
            message_error("\e[31mKo\n\e[0m");
}

int main(int argc, char **argv)
{
    // check if the arguments are correct
    check_error(argc, argv);
    // write a message
    puts("\e[34mCreate Tree :\e[0m");
    // create the directory of the result
    mkdir(argv[2], 0777);
    // create a structure *dir for use readdir()
    struct dirent *dir;
    // create a struct FILE* for use getline()
    FILE *file = NULL;
    // check if on the once arguments a slash at the end exist if not add it
    argv[1] = add_slash(argv[1]);
    // read the directory
    DIR *d = opendir(argv[1]);
    // loop for each file in the directory
    while ((dir = readdir(d))) {
        // check if the file end with .html
        if (extension(dir->d_name)) {
            // read the file with fopen()
            file = read_file(dir->d_name, argv[1]);
            // parse the file to create the tree
            parse_file(file, argv[2], add_two_string(argv[1], dir->d_name), add_two_string(argv[1], "attachments/"));
        }
    }
    // close the directory
    if (closedir(d))
        message_error("\e[31mKo\n\e[0m");
    puts("\e[32mOk\e[0m");
    puts("\e[34mConvert *.html on *.md :\e[0m");
    md_t *all_md = NULL;
    read_tree(argv[2], 0, &all_md, ".html");
    convert_html_to_md(all_md);
    puts("\e[32mOk\e[0m");
    puts("\e[34mCleaning :\e[0m");
    if (system(add_two_string(add_two_string("find ", replace_space(argv[2])), " -name \"*.html\" -delete")))
        message_error("\e[31mKo\n\e[0m");
    puts("\e[32mOk\e[0m");
    puts("\e[34mChange name on *.png:\e[0m");
    // destroy_list(&all_md);
    // read_tree(argv[2], 0, &all_md, ".md");
    // print_linked_list(all_md);
    return 0;
}
