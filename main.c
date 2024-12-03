//Author Khoi Vu

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkx.h>
#include <signal.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>


GtkWidget *window;
GtkWidget *textfield;
GtkTextBuffer *textBuffer;
GtkBuilder *builder;
GtkTextTag *textTag;
GtkTextIter start, end;
GtkTextIter iter;
char *command[20], cmds[100] = "";
extern int errno ;
int lineOffset = 0;


gboolean keyPressed(GtkWidget *widget, GdkEventKey *event, gpointer data);
void displayAfterEnterKey();
gboolean mouseClicked(GtkWidget *widget, GdkEventButton *event, gpointer data);
char* concat(const char *s2);

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

void breakString(char* input, char** newArray) {
    if(input[0] == '.') {
        newArray[0] = "./";
        newArray[1] = input;
        newArray[2] = NULL;
        return;
    }
    
    int i = 0;
    while(i < 20) {
        newArray[i] = strsep(&input, " ");

        if(input == NULL) {
            newArray[i + 1] = strsep(&input, " ");
            break;
        }
        i++;
    }
}

//cd - Change directory
//Use chdir()
void changeDirectory(char* dir, GtkTextIter *iter) {
    int errnum;
    if(dir != NULL){
        if(chdir(dir) == -1){
            errnum = errno;
            char error[100] = "bash: cd: ";
            strcat(error, dir);
            strcat(error, ": ");
            strcat(error, strerror( errnum ));
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, error, -1, "editability", NULL);
        } 
    }

    // display current directory
    displayAfterEnterKey(iter);
}

//pwd - Present Working directory
// Use getcwd()
void printCurrentDirectory(GtkTextIter *iter) {
    
    char s[100];
    char* dir = getcwd(s, sizeof(s));
    
    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n",-1, "editability", NULL);

    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, dir, -1, "editability", NULL);

    displayAfterEnterKey(iter);

}

//mkdir - Make a directory (Alerts if already exists)
//Use mkdir()
void makeDirectory(char* name, GtkTextIter *iter) {
    int value;
    value = mkdir(name, 0777);
    if(value == -1) {
        //printf("%s\n", strerror(errno));
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "\n",-1, "editability", NULL);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, strerror(errno), -1, "editability", NULL);
    }
    displayAfterEnterKey(iter);
}

//rmdir - Remove the directory (Alerts if no such file or directory)
//Use rmdir()
void removeDirectory(char* dirName, GtkTextIter *iter) {
    int status = rmdir(dirName);
    if(status == -1) {
        //printf("%s\n", strerror(errno));
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "\n",-1, "editability", NULL);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, strerror(errno), -1, "editability", NULL);
    }
    displayAfterEnterKey(iter);
}

//ls - List contents of pwd
//Look at dirent.h library
void lsCommand(char** args, GtkTextIter *iter, char* arg) {
    DIR *directory;
    int errnum;
    
    if(arg == NULL){
        
        directory = opendir(".");

        struct dirent *dp;

        if(directory) {
            while((dp = readdir(directory)) != NULL) {
                if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                    continue;
                }
                if(dp->d_type == DT_DIR){
                    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
                    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, dp->d_name, -1, "editability", "blue_fg", NULL);
                }
                else if (access(dp->d_name, X_OK) != -1) {
                    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
                    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, dp->d_name, -1, "editability", "green_fg", NULL);
                }
                else{
                    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
                    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, dp->d_name, -1, "editability", NULL);
                }
            }
        } else {
            errnum = errno;
            char error[100] = "ls: cannot access ";
            strcat(error, "'");
            strcat(error, arg);
            strcat(error, "':");
            strcat(error, strerror( errnum ));
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, error, -1, "editability", NULL);
            closedir(directory);
            return;
        }
        closedir(directory);
    }
    else {

        for(int i =1; args[i] != NULL; i++){
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, args[i], -1, "editability", NULL);
            
            directory = opendir(args[i]);
            struct dirent *dp;

            if(directory) {
                while((dp = readdir(directory)) != NULL) {
                    if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                        continue;
                    }
                    if(dp->d_type == DT_DIR){
                        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
                        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, dp->d_name, -1, "editability", "lmarg", "blue_fg", NULL);
                    }
                    else if (access(dp->d_name, X_OK) != -1) {
                        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
                        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, dp->d_name, -1, "editability", "lmarg", "green_fg", NULL);
                    }
                    else{
                        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
                        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, dp->d_name, -1, "editability", "lmarg", NULL);
                    }
                   
                }
            } else {
                errnum = errno;
                char error[100] = "ls: cannot access ";
                strcat(error, "'");
                strcat(error, args[i]);
                strcat(error, "':");
                strcat(error, strerror( errnum ));
                gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
                gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, error, -1, "editability", NULL);
                closedir(directory);
                return;
            }
            closedir(directory);

            gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
        }
       
    }
    
}

//cp - Copy contents from one file to another
//Use fgetc() and fputc()
void copyFile(char* parentName, char* childName) {
    FILE *parentFile, *childFile;

    parentFile = fopen(parentName, "rb");
    if (parentFile == NULL) {
        //perror("Error ");
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "\n",-1, "editability", NULL);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, strerror(errno), -1, "editability", NULL);
        return;
    }

    childFile = fopen(childName, "wb");

    if(childFile == NULL) {
        //perror("Error ");
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "\n",-1, "editability", NULL);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, strerror(errno), -1, "editability", NULL);
        return;
    }

    fseek(parentFile,0, SEEK_END);
    int fileLength = (int)ftell(parentFile);
    fseek(parentFile, 0, SEEK_SET);

    for(int i = 0; i < fileLength; i++) {
        fputc(fgetc(parentFile), childFile);
    }


    fclose(parentFile);
    fclose(childFile);
    

}

//exit
void exitCommand() {
    printf("/nEXIT/n");
    exit(0);
}

//
void runExecutable(char* execName, GtkTextIter *iter) {
    int fd = open("outPut.txt", O_WRONLY|O_CREAT, 0666);
    char *args[] = {execName, NULL};


    pid_t pid = vfork();

    if(pid == -1) {
        //printf("%s\n", strerror(errno));
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "\n",-1, "editability", NULL);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, strerror(errno), -1, "editability", NULL);
    } else if(pid == 0) {
        dup2(fd, 1);
        execvp(args[0], args);
    } else {
        wait(NULL);
        FILE *outputFile;
        char c[1000];

        outputFile = fopen("outPut.txt", "r");

        if(outputFile == NULL) {
            printf("%s\n", strerror(errno));
            return;
        }



        fseek(outputFile,0, SEEK_END);
        int fileLength = (int)ftell(outputFile);
        fseek(outputFile, 0, SEEK_SET);

        
        fgets(c, fileLength+1,outputFile);

        // while (c != EOF)
        // {
        //     //gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, &c, -1, "editability", NULL);
            
        //     c = fgets(outputFile);

        // }

        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n",-1, "editability", NULL);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, c, -1, "editability", NULL);

        fclose(outputFile);
        remove("outPut.txt");
    }

}

void readCommand(char** cmd, GtkTextIter *iter, char* cmds) { //char** array of strings
    int numOfCmds = 8;
    int ownCmd = 0;
    char* cmdList[numOfCmds];
    char* arr = ": command not found";

    cmdList[0] = "cd";
    cmdList[1] = "pwd";
    cmdList[2] = "mkdir";
    cmdList[3] = "rmdir";
    cmdList[4] = "ls";
    cmdList[5] = "cp";
    cmdList[6] = "./";
    cmdList[7] = "exit";


    if (sizeof(cmd) == 0) {
        //call method to restart newline
    }

    for (int i=0; i<numOfCmds; i++){
        if (strcmp(cmd[0], cmdList[i]) == 0){
            ownCmd = i + 1;
            break;
        }
    }
     
    switch (ownCmd) {
    case 1:
        if(cmd[2] == NULL){
            changeDirectory(cmd[1], iter); //cmd[1] should have the argument
        }
        else{
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "\n",-1, "editability", NULL);
            gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "bash: cd: too many arguments", -1, "editability", NULL);
        }
        break;
    case 2:
        printCurrentDirectory(iter);
        break;
    case 3:
        makeDirectory(cmd[1], iter);
        break;
    case 4:
        removeDirectory(cmd[1], iter);
        break; 
    case 5:
        lsCommand(cmd, iter, cmd[1]);
        displayAfterEnterKey(iter);
        break;
    case 6:
        copyFile(cmd[1], cmd[2]);
        displayAfterEnterKey(iter);
        break;
    case 7:
        runExecutable(cmd[1], iter);
        displayAfterEnterKey(iter);
        break;
    case 8:
        exitCommand();
        break;
    default:
        
        strcat(cmds, arr);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, "\n",-1, "editability", NULL);
        gtk_text_buffer_insert_with_tags_by_name(textBuffer, &end, cmds, -1, "editability", NULL);
        displayAfterEnterKey(iter);

        break;
    } 
}


//char *argv[] is an array of char pointers


int main(int argc, char *argv[] ) {
    
    char s[100];
    char* dir = getcwd(s, sizeof(s));

    strcat(dir, ":");

    gtk_init(&argc, &argv);

//----------------------------------------------------------------
// establish contact with xml code used to adjust widget settings
//----------------------------------------------------------------

    //any gtk method will return an address of the data structure.
    //Thus, to reference the data structure and not the address
    //of the data structure, pointers are used to point to the
    //actual data structure
    
    
    builder = gtk_builder_new_from_file("terminal.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "terminal_window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    textfield = GTK_WIDGET(gtk_builder_get_object(builder, "terminal_textfield"));

    // Add this code to set up dark theme
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "textview, textview text {"
        "   background-color: #000000;"  /* Dark gray background */
        "   color: #FFFFFF;"             /* White text */
        "   font-family: monospace;"
        "   caret-color: white;"   
        "}"
        "scrolledwindow {"
        "   background-color: #000000;"
        "}"
        , -1, NULL);
    
    GtkStyleContext *context = gtk_widget_get_style_context(textfield);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    // Also apply to the scrolled window parent
    GtkWidget *scrolled_window = gtk_widget_get_parent(textfield);
    context = gtk_widget_get_style_context(scrolled_window);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    g_object_unref(provider);

    textBuffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(textfield));

    gtk_text_buffer_create_tag(textBuffer, "editability", "editable", False, NULL);
    gtk_text_buffer_create_tag(textBuffer, "blue_fg", "foreground", "#00AFFF", NULL); 
    gtk_text_buffer_create_tag(textBuffer, "green_fg", "foreground", "#00FF00", NULL); 
    gtk_text_buffer_create_tag(textBuffer, "lmarg", "left_margin", 10, NULL);
    
    gtk_text_buffer_get_iter_at_offset(textBuffer, &iter, 0);
    
    char *username = getenv("USER");
    gtk_text_buffer_insert_with_tags_by_name(textBuffer, &iter, username, -1, "editability", "green_fg", NULL);
    char direc[100] = "~";
    strcat(direc, dir);
    gtk_text_buffer_insert_with_tags_by_name(textBuffer, &iter, direc, -1, "editability", "blue_fg", NULL);
    lineOffset = gtk_text_iter_get_line_offset(&iter);

    g_signal_connect(textfield, "key-press-event", G_CALLBACK(keyPressed), NULL);
    g_signal_connect(textfield, "button-press-event", G_CALLBACK(mouseClicked), NULL);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

gboolean keyPressed(GtkWidget *widget, GdkEventKey *event, gpointer data){
    
    char *temp;
    gtk_text_buffer_get_start_iter(textBuffer, &start);
    gtk_text_buffer_get_end_iter(textBuffer, &end);
    
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter)
    {
        gtk_text_buffer_apply_tag_by_name(textBuffer, "editability", &start, &end);

        breakString(cmds, command);
        readCommand(command, &end, cmds);

        memset(cmds,0,strlen(cmds));        
       
        return True;
    }

    if(event->keyval == GDK_KEY_Up || event->keyval == GDK_KEY_Down){
        return True;
    }

    if(event->keyval == GDK_KEY_Left){
        GtkTextIter temp;
        gtk_text_buffer_get_iter_at_mark(textBuffer, &temp, gtk_text_buffer_get_insert(textBuffer));
        int col = gtk_text_iter_get_line_offset(&temp);
        if((int) strlen(g_get_current_dir()) + 1 == col){
            return True;
        }
        return False;
    }
    if(event->keyval == GDK_KEY_slash){
        strcat(cmds, "/");
        return False;
    }

    if(event->keyval == GDK_KEY_BackSpace){
        GtkTextIter temp;
        gtk_text_buffer_get_iter_at_mark(textBuffer, &temp, gtk_text_buffer_get_insert(textBuffer));
        int col = gtk_text_iter_get_line_offset(&temp);
        
        int translatedOffset = (col % lineOffset) -1;
        
        memmove(&cmds[translatedOffset], &cmds[translatedOffset + 1], strlen(cmds) - translatedOffset);

        return False;
    }
    if(event->keyval == GDK_KEY_space){
        strcat(cmds, " ");
        return False;
    }
    if(event->keyval == GDK_KEY_Shift_L || event->keyval == GDK_KEY_Shift_R){
        return False;
    }
    if(event->keyval == GDK_KEY_period){
        strcat(cmds, ".");
        return False;
    }
    
    temp = gdk_keyval_name(event->keyval);
    strcat(cmds, temp);

    return False;
}


void displayAfterEnterKey(GtkTextIter *iter){
    char s[100];
    char* dir = getcwd(s, sizeof(s));
    strcat(dir, ":");
   
    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, "\n", -1, "editability", NULL);
    char *username = getenv("USER");
    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, username, -1, "editability", "green_fg", NULL);
    char direc[100] = "~";
    strcat(direc, dir);
    gtk_text_buffer_insert_with_tags_by_name(textBuffer, iter, direc, -1, "editability", "blue_fg", NULL);
    
    GtkTextIter temp;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &temp, gtk_text_buffer_get_insert(textBuffer));
    lineOffset = gtk_text_iter_get_line_offset(&temp);

    gtk_text_buffer_place_cursor (textBuffer, iter);
}

gboolean mouseClicked(GtkWidget *widget, GdkEventButton *event, gpointer data){
    if(event->type == GDK_BUTTON_PRESS && event->button == 1){
        return True;
    }
    return False;
}



