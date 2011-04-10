%%{	
	machine ws;
	
	uni = 0x80..0xbf;
	esc_seq = 	"\\\"" | "\\b" | "\\n" | "\\r" | "\\\\" | 
				"\\/" | "\\f" | "\\t" | ("\\u" xdigit{4});
	uni_ascii_area = ((0x20..0x7e) -- (0x22 | 0x5c)) | esc_seq;
	uni_area_1 = 0xc0..0xdf uni;
	uni_area_2 = 0xe0..0xef uni uni;
	uni_area_3 = 0xf0..0xf7 uni uni uni;
	uni_token = uni_ascii_area | uni_area_1 | uni_area_2 | uni_area_3;
	
	cr_lf = "\r\n";
	sp = " "*;
	
	uri = (alnum | '\\' | '.')+ >A_save_start %A_save_uri;
	
	request_line = "GET" sp uri sp "HTTP/1.1" cr_lf; #remake for accordance rfc2616 request_line
	
	sep = ":";
	sp_sep_sp = sp sep sp;
	
	key = (uni_token - ':')+ >A_save_start %A_save_key;
	value = (uni_token)+ >A_save_start %A_save_value;
	
	header = key sp_sep_sp value cr_lf;
	
	handshake = request_line (headers)* cr_lf (0x00..0xff){8} %A_handle_handshake;
	
	text_frame = 0x00 (uni_token)* 0xff >A_save_start %A_handle_message;
	
	closing_frame = 0xff 0x00 >%A_handle_closing_frame 
	
	frame = text_frame | closing_frame;
	
	main := handshake (frame)* $A_handle_error;

}%%
