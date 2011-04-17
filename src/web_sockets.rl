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
	
#-------------------------------------URI------------------------------------------------#	
	unreserved    = alnum | "-" | "." | "_" | "~";
	sub_delims    = "!" | "$" | "&" | "'" | "(" | ")" | "*" | "+" | "," | ";" | "=";
	pct_encoded   = "%" xdigit xdigit;
	pchar         = unreserved | pct_encoded | sub_delims | ":" | "@";
	query         = ( pchar | "/" | "?" )*;
	segment_nz = pchar+;
	segment = pchar*;
	relative_path = segment_nz ( "/" segment )*;
	absolute_path = "/" ( relative_path )?;
	uri = ((absolute_path | relative_path) ( "?" query )?) >A_save_start %A_save_uri;
	
	cr_lf = "\r\n";
	sp = " ";
	
	request_line = "GET" sp+ uri sp+ "HTTP/1.1" cr_lf; #remake for accordance rfc2616 request_line
	
	sep = ":";
	sp_sep_sp = sp* sep sp*;
	
	key = (uni_token - ':')+ >A_save_start %A_save_key;
	value = 0x20* ((uni_token - 0x20) uni_token*) >A_save_start %A_save_value;
	
	header = key ":" value cr_lf;
	
	challenge = (0x00..0xff){8} >A_save_start ;
	
	handshake = request_line (header)* cr_lf  challenge %A_handle_handshake;
	
	text_frame = (0x00 (uni_token)* 0xff) >A_save_start %to(A_handle_message);
	
	closing_frame = 0xff 0x00 %to(A_handle_closing_frame);
	
}%%
