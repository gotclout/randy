/* divert-loop.c

   loop of FreeBSD divert socket

   Xinyuan Wang 9/3/2000
*/

#define DEBUG

#include "divertlib.h"
#include <string.h>
#include <stdio.h>

#define maxbuflen 1520

#define maxiplen 64

char** getHostIps()
{
  char** hostipvec = (char**) malloc(sizeof(char*)*2);
  const char* cmd = "ifconfig | grep \"inet \" | sed 's/|/ /' |awk '{print $2}'";
  int i;
  FILE* f = (FILE*) popen(cmd, "r");
  if(f)
  {
    i = 0;
    hostipvec[i] = (char*) malloc(maxiplen);
    memset(hostipvec[i], 0, maxiplen);
    char line[maxiplen];
    while(fgets(line, sizeof(line), f) && i < 2)
    {
      strncpy(hostipvec[i], line, strlen(line) - 1);
#ifdef DEBUG
      printf("host ip: %d, %s", i, line);
#endif
      i++;
      hostipvec[i] = (char*) malloc(maxiplen);
      memset(hostipvec[i], 0, maxiplen);
    }
    pclose(f);
  }

  return hostipvec;
}

int srcIpMatchDstIp(struct ip* iph, char** hosts)
{
  char* srcip = (char*) malloc(maxiplen);
  memset(srcip, 0, maxiplen);
  char* dstip = (char*) malloc(maxiplen);
  memset(dstip, 0, maxiplen);

  char* srcstr = (char*)inet_ntoa(iph->ip_src);
  memcpy(srcip, srcstr, strlen(srcstr));
  char* dststr = (char*)inet_ntoa(iph->ip_dst);
  memcpy(dstip,dststr, strlen(dststr));

  if(strcmp(srcip, hosts[0]) == 0 || strcmp(srcip, hosts[1]) == 0)
  {
    if(strcmp(dstip, hosts[0]) == 0 || strcmp(dstip, hosts[1]) == 0)
    {
      return 0;
    }
  }

  return 1;
}

int main(int argc, char *argv[])
{
  int i, len, divsock;
  u_short iphlen, tcphlen;
  int udpsock, DivPort;
  struct sockaddr_in sin, sin1;
  struct ip *iph;
  struct tcphdr *tcph;
  struct udphdr *udph;
  unsigned char buf[maxbuflen+1];
  int addrsize=sizeof (struct sockaddr);

  if (argc!=2)
  {
    puts("usage : divert-loop [div port]");
    return 1;
  };

  DivPort=atoi(argv[1]);
  printf("DivPort=%d\n", DivPort);

  if ((divsock=initDivSock(DivPort))<=0)
  {
    printf("can not get divert socket for port %d, divsock=%d\n",
            DivPort, divsock);
    exit(1);
  };

  char** hosts = getHostIps();

  for (i=1; ;i++)
  {
    if ((len=recvfrom(divsock, buf, maxbuflen, 0,
       (struct sockaddr *) &sin, &addrsize))>0)
    {

      iph=(struct ip *) buf;
      iphlen=iph->ip_hl<<2;
#ifdef DEBUG
      if (sin.sin_addr.s_addr==INADDR_ANY) /* outgoing */
      { printf("\n%d : Out\t\t\t\t\t\t\t\t==>\n", i);
      }
      else /* incoming */
      { printf("\n%d : In from %s:%d\t\t\t\t\t\t<==\n",
                i, inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
      };
      printf("\tsrc IP:%s\n", inet_ntoa(iph->ip_src));
      printf("\tdst IP:%s\n", inet_ntoa(iph->ip_dst));
      printf("\tproto :%d\n", iph->ip_p);
#endif

      if(srcIpMatchDstIp(iph, hosts) == 0)
      {
        if(iph->ip_p == 1)
        {
          sendto(divsock, buf, len, 0, (struct sockaddr *) &sin, sizeof(struct sockaddr));
        }
      }
    }
  };

  if(hosts)
  {
    for(i = 0; i < 2; ++i)
    {
      free(hosts[i]);
    }
    free(hosts);
  }

  return 0;
}
