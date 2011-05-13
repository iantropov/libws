%%{
	machine web_sockets;
	
	alphtype unsigned char;
#-----------------------------------UNICODE------------------------------------------#
	uni = 0x80..0xbf;
	uni_ascii_area = 0x20..0x7e;
	uni_area_1 = 0xc0..0xdf uni;
	uni_area_2 = 0xe0..0xef uni uni;
	uni_area_3 = 0xf0..0xf7 uni uni uni;
	uni_token = uni_ascii_area | uni_area_1 | uni_area_2 | uni_area_3;
	
#-----------------------------------WEBSOCKETS---------------------------------------#
	text_frame = (0x00 (uni_token)* 0xff) >A_save_start %to(A_handle_message);
	closing_frame = 0xff 0x00 %to(A_handle_closing_frame);	
}%%
