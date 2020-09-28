#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "matreader.h"
#include <readline/readline.h>

/* 
 * A simple command tool to look at the contents of mat files
 * demonstrates how to use the matreader library
 *
 * */

void matbrowser(mde_t * root)
{

  printf("\n");
  printf("matbrowser()\n");
  printf("Select a variable by name or number\n");
  printf("use '..' to go down a level\n");
  printf("use Ctrl+d to quit\n");


  mde_t * node = root;

  int cont = 1;
  while(cont == 1)
  {
    if(node == root)
    {
      printf("At root (%d variables)\n", node->nchild);
    } else
    {
      mde_print_desc(node);
    }
    if(node->nchild == 0)
    {
      mde_print(node);
    }
    for(int kk = 0; kk<node->nchild; kk++)
    {
      if(node->child[kk]->name == NULL || strlen(node->child[kk]->name) == 0)
        printf("#%d ", kk);
      mde_print_desc(node->child[kk]);
    }

    char * line = readline("? ");
    if(line == NULL)
    { // eof: probably means Ctrl+d
      printf("\n");
      return;
    }

    if(strcmp(line, "..") == 0)
    {
      if(node->root == NULL)
      {
        printf("! Already a root level\n");
      } else {
        node = node->root;
      }
      goto next;
    }

    // Try to figure out what variable to visit by name
    int opt = -1;
    for(int kk = 0; kk<node->nchild; kk++)
    {
      char * name = mde_get_name(node->child[kk]);
      if(strcmp(line, name) == 0)
      {
        opt = kk;
      }
      free(name);
    }
    // If that fails, go by number
    if(opt == -1)
    {
      char * end;
      opt = strtol(line, &end, 10);
      if(end == line)
      {
        opt = -1;
      }
    }

    if(opt < 0 || opt >= node->nchild)
    {
      printf("! No child with that name or number\n");
    } else {
      node = node->child[opt];
    }

next:
    free(line);
  }
}


static void usage(int argc, char ** argv)
{
  printf("Standard usage: \n\t%s <file>\n", argv[0]);

  printf("Only parse the file and don't open the browser:\n\t %s <file> -\n", argv[0]);
}

int main(int argc, char ** argv)
{
  int browse = 1;
  if(argc > 2)
    browse = 0;

  if(argc < 2)
  {
    usage(argc, argv);
    return 1;
  }

  // Open the file and direct any messages from matreader
  // to stdout
  printf("Reading %s ... \n\n", argv[1]);
  mde_t * root = matreader(argv[1], stdout);
  if(root == NULL)
    return -1;

  if(browse)
    matbrowser(root); // Look at the data
  mde_free(root);

  return 0;
}
