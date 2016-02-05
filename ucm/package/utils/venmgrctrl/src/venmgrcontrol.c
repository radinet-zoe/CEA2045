#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include<string.h>
int main( int argc, char *argv[])  
{
   setuid( 0 );
   if(strcmp(argv[1],"start") == 0)
	   system( "/etc/init.d/venmgr start" );
   else if(strcmp(argv[1],"stop") == 0)
	   system( "/etc/init.d/venmgr stop" );
   else if(strcmp(argv[1],"enable") == 0)
   	system( "/etc/init.d/venmgr enable" );
   else if(strcmp(argv[1],"disable") == 0)
   	system( "/etc/init.d/venmgr disable" );
   else if(strcmp(argv[1],"activeevent") == 0)
	system("/usr/bin/php-cgi /opt/venmanager/utils/cronXmlActiveEvent.php");
   else if(strcmp(argv[1],"upgrade") == 0)
	{
		char dest[100] = "/opt/venmanager/utils/autoupgrade.sh ";
		strcat(dest,argv[2]);
		strcat(dest," ");
		strcat(dest,argv[3]);
		strcat(dest," ");
		strcat(dest,argv[4]);
		system(dest);
	}
  else if(strcmp(argv[1],"ftpscertdownload") == 0)
	{
		char dest[500]="curl ";
		strcat(dest,argv[2]);
		strcat(dest,"FTPS_Client_Certificates.tgz");
		strcat(dest," -u ");
		strcat(dest,argv[3]);
		strcat(dest,":");
		strcat(dest,argv[4]);
		strcat(dest," -o /opt/venmanager/utils/certificates/ftps/FTPS_Client_Certificates.tgz 2>&1");
		
		//curl $SERVER$FILENAME -u "$USERNAME:$PASSWORD" -o $TEMP_FILE  2>&1 >> $LOG
		system(dest);
	}
  else if(strcmp(argv[1],"vtncertdownload") == 0)
        {
                char dest[500]="/usr/bin/curl ";
                strcat(dest,argv[2]);
                strcat(dest,"VTN_Client_Certificates.tgz");
                strcat(dest," -u ");
                strcat(dest,argv[3]);
                strcat(dest,":");
                strcat(dest,argv[4]);
                strcat(dest," -o /opt/venmanager/utils/certificates/vtn/VTN_Client_Certificates.tgz 2>&1 ");
                system(dest);
        }
  else if(strcmp(argv[1],"wificonnect") == 0)
	{
		char ssidcmd[500]="";		
		strcat(ssidcmd,"uci set wireless.@wifi-iface[1].ssid=\"");
		strcat(ssidcmd,argv[2]);
		strcat(ssidcmd,"\"");
		system(ssidcmd);
		char ssidcmd1[500]="";
		strcat(ssidcmd1,"uci set wireless.@wifi-iface[1].key=");
		strcat(ssidcmd1,argv[3]);
		system(ssidcmd1);
		system("uci set wireless.@wifi-iface[1].mode=sta");
		system("uci commit");
		system("wifi");
	}
  else if(strcmp(argv[1],"wifidisconnect") == 0)
	{
		system("uci delete wireless.@wifi-iface[1].mode");
		system("uci commit");
		system("wifi");
	}
  else if(strcmp(argv[1],"iwlist") == 0)
	{
		system("iw dev wlan0 scan | grep SSID");
	}
  else if(strcmp(argv[1],"getappaswd") == 0)
	{
		system("uci get wireless.@wifi-iface[0].key");
	}	
  else if(strcmp(argv[1],"appaswd") == 0)
	{
		char ssidcmd[500]="";
		strcat(ssidcmd,"uci set wireless.@wifi-iface[0].key=");
		strcat(ssidcmd,argv[2]);
		system(ssidcmd);
		system("uci commit");
		system("wifi");
	}
   return 0;
}
