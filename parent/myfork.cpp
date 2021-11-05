#include "myfork.h"

myfork::myfork() {}

int
myfork::rfork()
{
  int x = -2;
  x = checkpoint();
  return x;
}

int
myfork::checkpoint()
{
  int x = -2;
  if (dmtcp_is_enabled()) {
    rr = dmtcp_checkpoint();
    if (rr == DMTCP_NOT_PRESENT) {
      x = -1;
      printf("** Error, DMTCP not running; checkpoint skipped ** \n");
    }
    if (rr == DMTCP_AFTER_CHECKPOINT) {
      rename_me();

      printf("** Parent remote forked here! **\n");
      x = 0;
    }
    if (rr == DMTCP_AFTER_RESTART) {
      printf("\n");
      printf("** Child continuing after remote fork! **\n");
      x = 1;
    }
  } else {
    printf(" dmtcp disabled -- nevermind\n");
  }

  return x;
}

void
copyFile(const std::string &fileNameFrom, const std::string &fileNameTo)
{
  std::ifstream in(fileNameFrom.c_str());
  std::ofstream out(fileNameTo.c_str());
  out << in.rdbuf();
  out.close();
  in.close();
}


void
myfork::rename_me()
{
  getcwd(cwd, sizeof(cwd));
  char oldname[] = "";

  if ((dir = opendir(cwd)) != NULL) {
    /* print all the files and directories within directory */


    while ((ent = readdir(dir)) != NULL) {
      string filename = "";

      filename = string(ent->d_name);
      if (filename.find(".dmtcp") != std::string::npos) {
        // strcpy(oldname, filename.c_str());
        rename(ent->d_name, "image.dmtcp");
      }
    }


    closedir(dir);
  } else {
    /* could not open directory */
    perror("");
  }

  string srcpath = "image.dmtcp";
  string destpath = "to_send/image.dmtcp";
  copyFile(srcpath, destpath);
}
