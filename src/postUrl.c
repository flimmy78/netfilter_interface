#include<stdio.h> 
#include"curl/curl.h"


size_t writer(void *buff, size_t size, size_t nmem, void *usrp)
{
	 
	puts((char*)buff );
	puts("\n"); 
	return size*nmem; 
	
}




int postUrl(char *url, char *xmlStr) 
{

	if(NULL  == url)
 	{
 				
 		printf("url is null!\n");
 		return -1;  
 	}
 			
 			
	 CURL *curl; 
	 CURLcode res; 
	 curl_global_init(CURL_GLOBAL_ALL); 
	 
	 curl = curl_easy_init(); 

	 if(curl)
	 {
	 	curl_easy_setopt(curl, CURLOPT_URL, url); 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, xmlStr); 
		
		#ifdef SKIP_PEER_VERIFICATION 
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 
		#endif
			
		#ifdef SKIP_hostName_VERICATION 
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); 	
		#endif
		
	  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		
		res =  curl_easy_perform(curl); 
		
		if(res != CURLE_OK) 
		{
			puts("res: fail to post\n");
			fprintf(stderr, "curl_easy_perform() failed :%s\n", curl_easy_strerror(res)); 
			return -1; 
		}

		curl_easy_cleanup(curl);			 
	 }
	 curl_global_cleanup(); 
	 return 0; 	
}
int main1(int argc , char **argv)
{
	if(argc != 3) 
	{	
		printf("parameters of command is error!\n"); 
		return -1; 
	}
	
	
	char url[50];
	char xmldata[200]; 
	strcpy(url, argv[1]); 
	strcpy(xmldata, argv[2]); 
	postUrl(url, xmldata); 

	return 0; 
} 

int main() 
{
	const char *url="https://192.168.3.165/";
	
	char *xmldata1=	"xml version=\"1.0\" encoding=\"utf-8\"?>   \
		<data direct=\"4\"cmd=\"4152\"subcmd=\"1\">   \
		<!--src_sms_id銆乨est_sms_id鏄畨绠¤浆鍙戝懡浠や娇鐢ㄧ殑瀛楁锛屽畨鍏ㄨ澶囧彧闇�鍒ゆ柇dest_sms_id鏄惁涓虹洿鎺ョ鐞嗗畨绠�->   \
		<src_sms_id>XXXXXXXXXX</src_sms_id>    \
		<dest_sms_id>XXXXXXXXXX</dest_sms_id >         \
			<dev_id>XXXXXXXXXXXXXXX</dev_id >         \
				<!--dev_id鏄畨鍏ㄨ澶囩殑鍞竴鏍囪瘑-->              \
				</data> " ; 
	char *xmldata2="<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
	<data direct=\"4\"cmd=\"4151\"subcmd=\"1\">   \
		<dev id=\"cxfv.id\"smsip=\"sysint.smsaddr\"ccip=\"\"smsid=\"sysint.smsid\"/></data>"; 
	
	char *xmldata3="<?xml version=\"1.0\" encoding=\"utf-8\"?> \
		<data direct=\"4\"cmd=\"4151\"subcmd=\"2\">   \
		<!--src_sms_id銆乨est_sms_id鏄畨绠¤浆鍙戝懡浠や娇鐢ㄧ殑瀛楁锛�\
		瀹夊叏璁惧闇�鍒ゆ柇dest_sms_id鏄惁涓虹洿鎺ョ鐞嗗畨绠�->     \
			<src_sms_id>XXXXXXXXXX</src_sms_id>      \
				<dest_sms_id>XXXXXXXXXX</dest_sms_id> \
				<dev_id>XXXXXXXXXXXXXXX</dev_id>  \
					<!--dev_id鏄畨鍏ㄨ澶囩殑鍞竴鏍囪瘑-->   \ 
					</data>             \
					"; 
	char *xmldata4="<?xml version=\"1.0\" encoding=\"utf-8\"?> \
	<data direct=\"4\"cmd=\"4151\"subcmd=\"3\">  \
	<!--src_sms_id銆乨est_sms_id鏄畨绠¤浆鍙戝懡浠や娇鐢ㄧ殑瀛楁锛� \
	瀹夊叏璁惧闇�鍒ゆ柇dest_sms_id鏄惁涓虹洿鎺ョ鐞嗗畨绠�->   \
		<src_sms_id>XXXXXXXXXX</src_sms_id>    \
			<dest_sms_id>XXXXXXXXXX</dest_sms_id>   \
			<dev_id>XXXXXXXXXXXXXXX</dev_id>    \
				<!--dev_id鏄畨鍏ㄨ澶囩殑鍞竴鏍囪瘑-->   \
				</data>       \
				"; 
	char *xmldata5="<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
	<data direct=\"4\"cmd=\"4151\"subcmd=\"4\">  \
	<!--src_sms_id銆乨est_sms_id鏄畨绠¤浆鍙戝懡浠や娇鐢ㄧ殑瀛楁锛� \
	瀹夊叏璁惧闇�鍒ゆ柇dest_sms_id鏄惁涓虹洿鎺ョ鐞嗗畨绠�->   \
		<src_sms_id>XXXXXXXXXX</src_sms_id>   \
			<dest_sms_id>XXXXXXXXXX</dest_sms_id>   \
			<dev_id>XXXXXXXXXXXXXXX</dev_id>   \
				<!--dev_id鏄畨鍏ㄨ澶囩殑鍞竴鏍囪瘑-->   \
				<new_ccenter_addrip=\"192.168.1.1\"/>   \
				</data>   \
				"; 
	char *xmldata6="<?xml version=\"1.0\" encoding=\"utf-8\"?>   \
	<data direct=\"4\"cmd=\"4152\"subcmd=\"2\">    \
		<!--src_sms_id銆乨est_sms_id鏄畨绠¤浆鍙戝懡浠や娇鐢ㄧ殑瀛楁锛屽畨鍏ㄨ澶囬渶瑕佸垽鏂璬est_sms_id鏄惁涓虹洿鎺ョ鐞嗗畨绠�->  \
			<src_sms_id>XXXXXXXXXX</src_sms_id>   \
				<dest_sms_id>XXXXXXXXXX</dest_sms_id> \ 
					<dev_id>XXXXXXXXXXXXXXX</dev_id>   \
						<!--dev_id鏄畨鍏ㄨ澶囩殑鍞竴鏍囪瘑-->  \
							<!--淇敼鏍煎紡鍚�.1.2鎵�垪鑳藉璁剧疆-->   \
							</data>    \ 
							";
	char *xmldata7="<?xml version=\"1.0\" encoding=\"utf-8\"?>   \
		<data direct=\"4\"cmd=\"4153\"subcmd=\"1\">    \
		<!--src_sms_id銆乨est_sms_id鏄畨绠¤浆鍙戝懡浠や娇鐢ㄧ殑瀛楁锛屽畨鍏ㄨ澶囧彧闇�鍒ゆ柇dest_sms_id鏄惁涓虹洿鎺ョ鐞嗗畨绠�->  \
		<src_sms_id>XXXXXXXXXX</src_sms_id>   \
		<dest_sms_id>XXXXXXXXXX</dest_sms_id>   \
			<dev_id>XXXXXXXXXXXXXXX</dev_id>   \
				<!--dev_id鏄畨鍏ㄨ澶囩殑鍞竴鏍囪瘑-->  \
				</data>   \
				"; 
	postUrl(url, xmldata1); 
	postUrl(url, xmldata2); 
	postUrl(url, xmldata3); 
	postUrl(url, xmldata4);	
	postUrl(url, xmldata5); 
	postUrl(url, xmldata6); 
	postUrl(url, xmldata7); 


	
}
