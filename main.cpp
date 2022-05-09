#include <iostream>
#include <string>
#include <fstream>


#define ERR_EXPECTED_INSTRUCTION 1
#define ERR_EXPECTED_ADRESS 2
#define ERR_EXPECTED_HEXCODE 3
#define ERR_EXPECTED_ARG 4
#define ERR_EXPECTED_INT 5
#define ERR_EXPECTED_VALID_BUFFER 6


std::string hexmap = "0123456789ABCDEF";


class Addr
{
public:
  short addr;
  short buf;
};


int buf0[256];
char buf1[256];
int buf2[16];
char buf3[16];
int buf4[16];
Addr buf5[256];


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


void err(const char * str)
{
  std::cerr << "\003[1;31mErr:\033[0m " << str << std::endl;
  std::cerr << "Failed to run!" << std::endl;
}


int readaddr(std::string str, int i, int j, int k, Addr * out)
{
  i++;
  k++;
  out->addr = indexof(str[i], hexmap);
  if (out->addr == -1)
  {
    err("Expected Hexadecimal value (letters should be caps) 3", j, k);
    return -1;
  }
  out->addr *= 16;
  i++;
  k++;
  out->buf = indexof(str[i], hexmap);
  if (out->buf == -1)
  {
    err("Expected Hexadecimal value (letters should be caps) 4", j, k);
    return -1;
  }
  out->addr += out->buf;
  i++;
  k++;
  out->buf = indexof(str[i], hexmap);
  if (out->buf == -1)
  {
    err("Expected Hexadecimal value (letters should be caps) 5", j, k);
    return -1;
  }
  return 0;
}


int readint(std::string str, int i, int j, int k)
{
  k++;
  i++;
  char buf = str[i];
  if ((int)buf < 48 || (int)buf > 57)
  {
    err("Expected Positive Integer", j, k);
    return -1;
  }
  int buf2 = ((((int)buf - 48) * 16) * 16) * 16;
  k++;
  i++;
  buf = str[i];
  if ((int)buf < 48 || (int)buf > 57)
  {
    err("Expected Positive Integer", j, k);
    return -1;
  }
  buf2 += (((int)buf - 48) * 16) * 16;
  k++;
  i++;
  buf = str[i];
  if ((int)buf < 48 || (int)buf > 57)
  {
    err("Expected Positive Integer", j, k);
    return -1;
  }
  k++;
  i++;
  buf = str[i];
  if ((int)buf < 48 || (int)buf > 57)
  {
    err("Expected Positive Integer", j, k);
    return -1;
  }
  buf2 += ((int)buf - 48) * 16;
  k++;
  i++;
  buf = str[i];
  if ((int)buf < 48 || (int)buf > 57)
  {
    err("Expected Positive Integer", j, k);
    return -1;
  }
  buf2 += (int)buf - 48;
  return buf2;
}


int readchar(std::string str, int i, int j, int k, char * out)
{
  k++;
  i++;
  char buf = str[i];
  /*
  if ((int)buf < 32 || (int)buf == 127)
  {
    return -1;
  }
  */
  *out = buf;
  return 0;
}


int setbuffer(Addr addr, int i)
{
  if (addr.buf == 4)
  {
    buf4[addr.addr] = 1;
    return 0;
  }
  else if (addr.buf == 2)
  {
    buf2[addr.addr] = i;
    return 0;
  }
  else if (addr.buf == 0)
  {
    buf0[addr.addr] = i;
    return 0;
  }
  else
  {
    err("Cannot set buffer 1, 4 or 5 to an int!");
    return -1;
  }
}


int setbuffer(Addr addr, char c)
{
  if (addr.buf == 1)
  {
    buf1[addr.addr] = c;
    return 0;
  }
  else if (addr.buf == 3)
  {
    buf3[addr.addr] = c;
    return 0;
  }
  else
  {
    err("Cannot set buffer 0, 2, 4 or 5 to an adress!");
    return -1;
  }
}


int setbuffer(Addr addr, Addr in)
{
  if (addr.buf == 5)
  {
    buf5[addr.addr] = in;
    return 0;
  }
  else
  {
    err("Can only set buffer 5 to an adress!");
    return -1;
  }
}


int readhex(std::string str, int i, int j, int k)
{
	i++;
	k++;
	int buf = indexof(str[i], hexmap);
	if (buf == -1)
	{
		err("Expected Hexadecimal value (letters should be caps) 1", j, k);
		return -1;
	}
	buf *= 16;
	i++;
	k++;
	int buf2 = indexof(str[i], hexmap);
	if (buf2 == -1)
	{
		err("Expected Hexadecimal value (letters should be caps) 2", j, k); // Crash here
		return -1;
	}
	buf += buf2;
	return buf;
}


int main(int argc, char *argv[])
{
  bool callflags[] = {false, false};
  int prgmflags[256][2];
  int temp = 0;
  char temp2 = '\0';
  std::string filename;
  std::ifstream f;
  std::string line;
  std::string fstr;
  char cmd = '\0';
  std::string cmdparams;
  std::string commands = "^|[!]+-)/\\(~#";
  std::string scommands = "#)";
  std::string ccommands = "<=>";
  Addr selected;
  Addr selected2;
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
			std::cout << i << " " << j << " " << k << std::endl;
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
          err("Invalid Syntax", j, k);
          return ERR_EXPECTED_INSTRUCTION;
        }
      }
      if (cmd == '^')
      {
        do
        {
          i++;
          k++;
					if (fstr[i] == '\n')
					{
						k = 0;
						j++;
					}
        } while(fstr[i] == ' ' || fstr[i] == '\n');
        if (fstr[i] != '&' && fstr[i] != '*')
        {
          err("Invalid Syntax", j, k);
          return ERR_EXPECTED_ADRESS;
        }
        else if (fstr[i] == '&')
        {
          temp = readaddr(fstr, i, j, k, &selected);
          if (temp == -1)
          {
            return ERR_EXPECTED_HEXCODE;
          }
          i += 4;
          k += 4;
          if (fstr[i] != ';')
          {
            err("Invalid Syntax", j, k);
            return ERR_EXPECTED_ARG;
          }
          i++;
          k++;
          if (fstr[i] == '\'')
          {
            readchar(fstr, i, j, k, &temp2);
            i++;
            k++;
            setbuffer(selected, temp2);
          }
          else if (fstr[i] == '$')
          {
            temp = readint(fstr, i, j, k);
            if (temp == -1)
            {
              return ERR_EXPECTED_INT;
            }
            i+=4;
            k+=4;
            setbuffer(selected, temp);
          }
          else if (fstr[i] == '&')
          {
            temp = readaddr(fstr, i, j, k, &selected2);
            if (temp == -1)
            {
              return ERR_EXPECTED_HEXCODE;
            }
            i+=3;
            k+=3;
            setbuffer(selected, selected2);
          }
          else if (fstr[i] == '*')
          {
            temp = readaddr(fstr, i, j, k, &selected2);
            if (temp == -1)
            {
              return ERR_EXPECTED_HEXCODE;
            }
            i+=3;
            k+=3;
            if (selected2.buf != 5)
            {
              err("Cannot get value of non adress!", j, k);
              return ERR_EXPECTED_ADRESS;
            }
            selected2 = buf5[selected2.addr];
            setbuffer(selected, selected2);
            //setbuffer(selected, )
          }
        }
        cmd = '\0';
      }
			else if (cmd == ')')
			{
				while(fstr[i] == ' ' || fstr[i] == '\n')
        {
          i++;
          k++;
					if (fstr[i] == '\n')
					{
						k = 0;
						j++;
					}
        }
				temp = readhex(fstr, i, j, k);
				if (temp == -1)
				{
					return ERR_EXPECTED_HEXCODE;
				}
				i+=2;
				k+=2;
				prgmflags[temp][0] = i;
				prgmflags[temp][1] = k;
				cmd = '\0';
			}
			else if (cmd == '/')
			{
				do
        {
          i++;
          k++;
					if (fstr[i] == '\n')
					{
						k = 0;
						j++;
					}
        } while(fstr[i] == ' ' || fstr[i] == '\n');
				temp = readhex(fstr, i, j, k);
				if (temp == -1)
				{
					return ERR_EXPECTED_HEXCODE;
				}
				i = prgmflags[temp][0];
				k = prgmflags[temp][1];
				cmd = '\0';
			}
			else if (cmd == '[')
			{
				do
        {
          i++;
          k++;
					if (fstr[i] == '\n')
					{
						k = 0;
						j++;
					}
        } while(fstr[i] == ' ' || fstr[i] == '\n');
				if (fstr[i] == '&')
				{
					readaddr(fstr, i, j, k, &selected);
					switch (selected.buf)
					{
						case 0:
							std::cout << buf0[selected.addr];
							break;
						case 1:
							std::cout << buf1[selected.addr];
							break;
						case 2:
							std::cout << buf2[selected.addr];
							break;
						case 3:
							std::cout << buf3[selected.addr];
							break;
						case 4:
							std::cout << buf4[selected.addr];
							break;
						case 5:
							std::cout << buf5[selected.addr].addr << ", " << buf5[selected.addr].buf;
							break;
						default:
							err("Expected a valid buffer", j, k);
							return ERR_EXPECTED_VALID_BUFFER;
					}
					i+=3;
					k+=3;
					cmd = '\0';
				}
				else if (fstr[i] == '*')
				{
					std::cout << ":__:" << i << " " << j << " " << k << std::endl;
					readaddr(fstr, i, j, k, &selected);
					std::cout << ":__:" << i << " " << j << " " << k << std::endl;
					i+=3;
					k+=3;
					if (selected.buf != 5)
					{
						err("Cannot get value of non adress!", j, k);
						return ERR_EXPECTED_ADRESS;
					}
					selected = buf5[selected.addr];
					switch (selected.buf)
					{
						case 0:
							std::cout << buf0[selected.addr];
							break;
						case 1:
							std::cout << buf1[selected.addr];
							break;
						case 2:
							std::cout << buf2[selected.addr];
							break;
						case 3:
							std::cout << buf3[selected.addr];
							break;
						case 4:
							std::cout << buf4[selected.addr];
							break;
						case 5:
							std::cout << buf5[selected.addr].addr << ", " << buf5[selected.addr].buf;
							break;
						default:
							err("Expected a valid buffer", j, k);
							return ERR_EXPECTED_VALID_BUFFER;
					}
					cmd = '\0';
				}
			}
			else if (cmd == '+')
			{
				temp = readaddr(fstr, i, j, k, &selected);
				i+=3;
				k+=3;
				if (temp == -1)
				{
					return ERR_EXPECTED_ADRESS;
				}
				switch (selected.buf)
				{
					case 0:
						buf0[selected.addr]++;
						break;
					case 1:
						buf1[selected.addr] = (char)((int)buf1[selected.addr]+1);
						break;
					case 2:
						buf2[selected.addr]++;
						break;
					case 3:
						buf3[selected.addr] = (char)((int)buf3[selected.addr]+1);
						break;
					case 4:
						buf4[selected.addr]++;
						break;
					case 5:
						buf5[selected.addr].addr++;
						break;
					default:
						err("Expected a valid buffer", j, k);
						return ERR_EXPECTED_VALID_BUFFER;
				}
				cmd = '\0';
			}
      k++;
    }
  }
  f.close();
  //std::cout << buf1[0] << buf1[1] << buf1[2] << buf1[3] << buf1[4] << buf1[5] << buf1[6] << buf1[7] << buf1[8] << buf1[9] << buf1[10] << std::endl;
}
