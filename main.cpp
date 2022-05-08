#include <iostream>
#include <string>
#include <fstream>


std::string hexmap = "0123456789ABCDEF";


struct Addr
{
  short addr;
  short buf;
};


bool in(char c, std::string str)
{
  for (int i = 0; i < str.length(); i++)
  {
    if (str[i] == c)
    {
      return true;
    }
  }
  return false;
}


int indexof(char c, std::string str)
{
  for (int i = 0; i < str.length(); i++)
  {
    if (str[i] == c)
    {
      return i;
    }
  }
  return -1;
}


void err(const char * str, int l, int c)
{
  std::cerr << "\033[1;31mErr:\033[0m " << str << " Line no:" << l << " Character no:"  << c << std::endl;
  std::cerr << "Failed to run!" << std::endl;
}


int readaddr(std::string str, int i, int j, int k, Addr * out)
{
  i++;
  k++;
  out->addr = indexof(str[i], hexmap);
  if (out->addr == -1)
  {
    err("Expected Hexadecimal value (letters should be caps)", j, k);
    return -1;
  }
  out->addr *= 16;
  i++;
  k++;
  out->buf = indexof(str[i], hexmap);
  if (out->buf == -1)
  {
    err("Expected Hexadecimal value (letters should be caps)", j, k);
    return -1;
  }
  out->addr += out->buf;
  i++;
  k++;
  out->buf = indexof(str[i], hexmap);
  if (out->buf == -1)
  {
    err("Expected Hexadecimal value (letters should be caps)", j, k);
    return -1;
  }
  return 0;
}


int main(int argc, char *argv[])
{
  bool callflags[] = {false, false};
  int prgmflags[512];
  int temp = 0;
  std::string filename;
  std::ifstream f;
  std::string line;
  std::string fstr;
  char cmd = '\0';
  std::string cmdparams;
  std::string commands = "^|[!]+-)/\\(~#";
  std::string scommands = "#)";
  std::string ccommands = "<=>";
  int buf0[256];
  char buf1[256];
  int buf2[16];
  char buf3[16];
  int buf4[16];
  Addr buf5[256];
  Addr selected;
  for (int i = 1; i < argc; i++) // Process Arguments
  {
    if (argv[i][0] == '-')
    {
      // Option
      for (int j = 1; argv[i][j] != '\0'; j++)
      {
        if (argv[i][j] == 'c')
        {
          callflags[0] = true;
        }
        else if (argv[i][j] == 'r')
        {
          callflags[1] = true;
        }
      }
    }
    else
    {
      filename = argv[i];
    }
  }
  std::cout << callflags[0] << callflags[1] << std::endl;
  std::cout << filename << std::endl;
  f.open(filename.c_str());
  if (!f)
  {
    std::cout << "File not found: " << filename << std::endl;
  }
  else
  {
    while (getline(f, line))
    {
      fstr.append(line);
      fstr.append("\n");
    }
    int i = 0;
    int j = 1;
    int k = 1;
    std::cout << fstr << std::endl;
    for (int i = 0; i < fstr.length(); i++)
    {
      std::cout << i << " " << j << ' ' << k << ' ' << fstr[i] << std::endl;
      if (fstr[i] == '\n')
      {
        j++;
        k = 0;
        continue;
      }
      if (cmd == '\0')
      {
        if (in(fstr[i], commands))
        {
          cmd = fstr[i];
        }
        else if (fstr[i] != ' ' && fstr[i] != '\n')
        {
          err("Expected instruction!", j, k);
          return 1;
        }
      }
      if (cmd == '^')
      {
        do
        {
          i++;
          k++;
        } while(fstr[i] == ' ' || fstr[i] == '\n');
        if (fstr[i] != '&')
        {
          err("Expected adress", j, k);
          return 2;
        }
        else
        {
          temp = readaddr(fstr, i, j, k, &selected);
          if (temp == -1)
          {
            return 3;
          }
          i += 3;
          k += 3;
          std::cout << selected.addr << ' ' << selected.buf << std::endl;
        }
        cmd = '\0';
      }
      k++;
    }
  }
  f.close();
}
