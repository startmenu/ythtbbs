var conlist = new Array();
var quotelist = new Array();
var threadlist = new Array();
var othreadlist = new Array();
var titlelist = new Array();
var docliststr = '';
var doclistcurrentnum = -1;
var narrow;
var lasttitle = '';
var contentbg = 0;
var docmode = 0;
var doccurrmin = -1;
var doccurrmax = 0;
var floor = 0;

function con_show_head(mybbsname, secname, secstr, boardname, 
	boardnum, redirect) {
	narrow = document.body.offsetWidth < 800 ? 1 : 0;
	document.write('<div id="con_head">');
	document.write('<div id="con_head_nav">\
		[<a href="'+baseurl+'boa?secstr=" name="����'+mybbsname+'����">'
		+mybbsname+'����</a> &gt;&gt; ' +(!narrow ? '\
		<a href="'+baseurl+'boa?secstr='+secstr+'" name="����'+secname+'��">'
		+secname+'</a> &gt;&gt; ' : '')+'<a id="con_back_board" ' 
		+'href="" name="����'+boardname+'��">'
		+boardname+'</a>]</div>');
	//checkFrame(redirect);
	document.write('<span id="con_head_show_qmd"><a href="#" \
		onclick="con_switch_qmd(0);" name="��������ǩ����">\
		����ǩ����</a></span> \
		<span id="con_head_show_quote"><a href="#" \
		onclick="con_switch_quote(0);" name="������������">\
		��������</a></span> \
		<span id="con_head_show_right"><a href="#" \
		onclick="con_switch_right(0);" name="�����Ҳ�˵�">\
		�����Ҳ�˵�</a></span>'+'</div><div class="clear"></div>' );
	//hide this line
	/* +(!narrow ? '\
			<a href="'+baseurl+'doc?B=BMApplication" name="�������">�������</a>' :
			'')
	*/
}



function con_show_connav(num, myreply_mode,filename, thread, boardnum, isauthor, star, 
	voted, st, isthread, prevnum, nextnum, prevfile, nextfile, prevedit, 
	nextedit, inndboard, noreply, outgoing, anony, top) {
	/* st = 0, 1	bbscon.c */
	/* st = 2 	bbstcon.c */
	/* st = 3	bbsnt.c */
	
	var ftime = filename.split('.')[1];
	var evastr = '<a name="��������" \
		href="'+baseurl+'eva?B='+boardnum+'&F=M.'+ftime+'.A&star=';
	var docmode, str;
	if (conlist.length == 0) {
		conlist[conlist.length] = ftime;
	} else if (conlist[conlist.length - 1] != ftime) {
		conlist[conlist.length] = ftime;
	}

	var style = (conlist.length + 1) % 2;

	document.write('<div id="con_'+ftime+'_'+(top ? 'top' : 'bot')+'nav" \
		class="'+(top ? 'con_topnav_'+style : 'con_botnav_'+style )+
		'"><div class="con_nav_padding_'+style+'"><div class="'
		+(top ? 'con_topinfo_'+style : 'con_botinfo_'+style)+'">');
	if (top)
		document.write('[���� <a href="#here" name="������" \
			onclick="con_zoom('+ftime+', 28);">��</a> \
			<a href="#here" name="������" onclick="con_zoom('
			+ftime+', 14);">��</a> <a href="#here" name="С����" \
			onclick="con_zoom('+ftime+', 12);">С</a>] ');
	if (!narrow || (st == 2 && !top) || (narrow && top && st != 2)
		|| (st == 3 && (!top && !isauthor))) {
		document.write('[����('+ Math.round(star / 50 * voted)+'/'+voted+'��)');
//		if (enmysql) {
			document.write(' '+evastr+'1">һ��</a> '
				+evastr+'3">�ܺ�</a> '
				+evastr+'5">����</a>');
//		}
		document.write(']');
	}
	if (top && ((isthread && st == 2) || floor)) {
		document.write(' ['+(conlist.length == 1 ? '¥��': 
			(conlist.length)+'¥')+']');
		floor = 1;
	} 
	else if (top==0)
		document.write(' [<a href="#top" \
	    name="����ҳ�涥��">����</a>]');
	document.write('</div>');
	if (!noreply) {
    if (myreply_mode) {
		document.write('[<a href="'+baseurl+'pst?B='+boardnum+'&F=M.'
			+ftime+'.A&num='+num+(inndboard ? 'la=1': '')+
			'" name="�ظ�">�ظ�</a>]');
		} else { 
		document.write('[<a href="#here" onclick=\
		"con_reply_box_init('
		+ftime+', '+style+', '+inndboard+', '+outgoing+', '
		+anony+', '+boardnum+', '+num+', '+thread+');">�ظ�</a>] ');
		}
	}
	if ((top || !top) || st == 2)
		document.write('[<a href=\
			"'+baseurl+'ccc?B='+boardnum+'&amp;F=M.'+ftime+'.A" \
			name="ת����������">ת��</a>] ');
	if ((top || !top) || (st == 2 && (!isauthor || !narrow)))
		document.write('[<a href="'+baseurl+'fwd?B='+boardnum
			+'&amp;F='+filename+'" \
			name="�ĵ�վ������">�Ƽ�������</a>] ');
	if (isauthor && ((narrow && !top) || !narrow)) {
		document.write('[<a href="'+baseurl+'edit?B='+boardnum+
			'&F=M.'+ftime+'.A" name="�޸�����">�޸�</a>]\
			[<a href="'+baseurl+'del?B='+boardnum+'&F=M.'
			+ftime+'.A" onclick="return confirm(\
			\'���Ҫɾ����ƪ���£�\');"  name="ɾ��">ɾ��</a>] ');
	}
	switch (st) {
		case 0:
			if (prevnum >= 0 && !top)
				document.write('[<a href="'+baseurl+'con_'
					+boardnum+'_'+prevfile+
					'.htm" \
					name="��ƪ">��ƪ</a>] ');
			if (nextnum >= 0 && !top)
				document.write('[<a href="'+baseurl+'con_'
					+boardnum+'_'+nextfile+
					'.htm" \
					name="��ƪ">��ƪ</a>] ');
			if (!top || (top && !narrow)) {
				str = '[<a href="'+baseurl+'tcon_'+boardnum;
				str += '_'+thread+'.htm';
				if (isthread) 
					str += '?start='+(num + 1);
				str += '" name=';
				str += '"ͬ����չ��">';
				str += 'ͬ����';
				if (isthread) {
					str += 'չ��</a>] ';
				} else {
					str += '</a>/<a href="'+baseurl;
					str += 'tcon_';
					str += boardnum+'_'+thread;
					str += '.htm?start='+(num + 1)+'" ';
					str += 'name="������չ��">������';
					str += '</a>չ��] ';
				}
				document.write(str);
			}
			break;
		case 1:
			if (!isauthor && !top) {
				if (prevnum >= 0)
					document.write('[<a href="'+baseurl+'con_'
					+boardnum+'_'+prevfile+
					'.htm" \
					name="ͬ������ƪ">ͬ������ƪ</a>] ');
				if (nextnum >= 0)
					document.write('[<a href="'+baseurl+'con_'
					+boardnum+'_'+nextfile+
					'.htm" \
					name="ͬ������ƪ">ͬ������ƪ</a>] ');
			}
			if (!top) {
				str = '[<a href="'+baseurl+'tcon_'+boardnum;
				str += '_'+thread+'.htm';
				if (isthread)
					str += '?start='+(num + 1);
				str += '" name="ͬ����չ��">ͬ����';
				if (isthread){
					str += 'չ��</a>] ';
				} else {
					str += '</a>/<a href="'+baseurl;
					str += 'tcon_';
					str += boardnum+'_'+thread+'.htm?start=';
					str += (num + 1)+'" name="';
					str += '������չ��">������</a>';
					str += 'չ��] ';
				}
				document.write(str);
			}
			break;
		case 3:
			if (!top || !narrow) {
				str = '[<a href="'+baseurl+'tcon_'+boardnum;
				str += '_'+thread+'.htm';
				if (isthread)
					str +='?start='+(num + 1);
				str += '" name="ͬ����">ͬ����';
				if (isthread) {
					str += 'չ��</a>] ';
				} else {
					str += '</a>/<a href="'+baseurl;
					str += 'tcon_';
					str += boardnum+'_'+thread;
					str += '.htm?start='+(num + 1)+'" ';
					str += 'name="������չ��">';
					str += '������</a>չ��] ';
				}
				document.write(str);
			}
			break;
	}
	document.write('[<a href="'+baseurl+'doc?B='+boardnum+'">���ذ���</a>]  </div></div>');
	if (st == 1 || st == 0)
		docmode = 0;
	else
		docmode = 1;
	document.getElementById('con_back_board').href = baseurl+ 'doc?B='+boardnum+
		'&S='+(num - 4)+'&M='+docmode+(docmode ? '&D=1' : '');
}

function con_switch_qmd(show) {
	if (show) {
		for (i = 0; i < conlist.length; i++) {
			obj = document.getElementById(
				'con_'+conlist[i]+'_content_qmd');
			if (obj) {
				obj.style.display = '';
			}
		}
		document.getElementById('con_head_show_qmd').innerHTML = 
			'<a href="#" onclick="con_switch_qmd(0);" name=\
			"��������ǩ����">����ǩ����</a>';
	} else {
		for (i = 0; i < conlist.length; i++) {
			obj = document.getElementById(
				'con_'+conlist[i]+'_content_qmd');
			if (obj) {
				obj.style.display = 'none';
			}
		}
		document.getElementById('con_head_show_qmd').innerHTML = 
			'<a href="#" onclick="con_switch_qmd(1);" name=\
			"��ʾ����ǩ����">��ʾǩ����</a>';
	}
}

function con_switch_quote(show) {
	var obj;
	if (show) {
		for (i = 0; i < quotelist.length; i++) {
			obj = document.getElementById(quotelist[i]);
			if (obj)
				obj.style.display = '';
		}
		document.getElementById(
			'con_head_show_quote').innerHTML = 
			'<a href="#" onclick="con_switch_quote(0);" \
			name="������������">��������</a>';
	} else {
		for (i = 0; i < quotelist.length; i++) {
			obj = document.getElementById(quotelist[i]);
			if (obj)
				obj.style.display = 'none';
		}
		document.getElementById(
			'con_head_show_quote').innerHTML = 
			'<a href="#" onclick="con_switch_quote(1);" \
			name="��ʾ��������">��ʾ����</a>';
	}
}

function con_switch_right(show) {
	var obj1 = document.getElementById('con_main_right');
	var obj2 = document.getElementById('con_main_left');
	var obj3 = document.getElementById('con_head_show_right');
	var cookieitem = new CJL_CookieUtil("yjrg", 14, "/", location.host, false);
	if (!obj1 || !obj2 || !obj3)
		return;

	if (!show) {
		obj1.style.display = 'none';
		obj2.style.width = '100%';
		obj3.innerHTML = '<a href="#" name="��ʾ�Ҳ�˵�" \
			onclick="con_switch_right(1);">��ʾ�Ҳ�˵�</a>';
		if (cookieitem)
			cookieitem.setSubValue("con_hideright", 1);
	} else {
		obj2.style.width = '79%';
		obj1.style.display = '';
		obj3.innerHTML = '<a href="#" name="�����Ҳ�˵�" \
			onclick="con_switch_right(0);">�����Ҳ�˵�</a>';
		if (cookieitem)
			cookieitem.setSubValue("con_hideright", 0);
	}
}

function con_switch_right_bycookie() {
	var cookieitem = new CJL_CookieUtil("yjrg", 14, "/", location.host, false);
	if (cookieitem)
		con_switch_right(!cookieitem.getSubValue("con_hideright"));
}	

function con_check_width() {
	if (document.body.offsetWidth < 780)
		con_switch_right(0);
}


function con_doclist_item(num, flag, author, fname, edt, title, 
	size, star, neval) {
	var str = '';
	var len = 0;
	var tmpauthor, tmpcontentbg;
	var white = '';

	contentbg = (contentbg + 1) % 2;
	tmpcontentbg = contentbg;
	if (fname.split('.')[1] == ftime) {
		tmpcontentbg = contentbg;
		contentbg = 2;
	}
	doclistcurrentnum = num;
	
	tmpauthor = author.split('');
	for (i = 0; i < tmpauthor.length; i++) {
		if (/^[^\x00-\xff]$/.test(tmpauthor[i]))
			len += 2;
		else
			len += 1;
	}

	while (len < 12) {
		white = white + '&nbsp;';
		len++;
	}

	if(title.length != 0)
		lasttitle = titleStr(title);
	str = '<tr class="con_rel_tr_'+contentbg+'">';
	str += '<td class="con_rel_td_num_'+contentbg+'">'+num+'</td>';
	str += '<td class="con_rel_td_flag_'+contentbg+'">'+flag+'</td>';
	if(author.indexOf('.') == -1)
		str += '<td class="con_rel_td_owner_'+contentbg+'">\
			<a href="'+baseurl+'qry?U='+author+'" \
			name="��ѯ�û���'+author+'">'+author+'</a>'
			+white+'</td>';
	else
		str += '<td class="con_rel_td_owner_'+contentbg+'">'
			+author+white+'</td>';
	str += '<td class="con_rel_td_date_'+contentbg+'">\
		<nobr>'+shortFDate(fname)+'</nobr></td>';
	str += '<td class="con_rel_td_title_'+contentbg+'">\
		<div class="overflow">';
	if (ftime == fname.split('.')[1] && !docmode)
		str += lasttitle + sizeStr(size) + '</div></td>';
	else {
		if (!docmode)
			str += '<a href="'+baseurl+'con_'+board+'_'+fname
				+'.htm" name=\
				"'+lasttitle+'">'+lasttitle+
				'</a>'+sizeStr(size)+'</div></td>';
		else
			str += '<a href="'+baseurl+'bbsnt?B='+board+'&start='+num+
				'&th='+fname.split('.')[1]+
				'" name="'+lasttitle+
				'">'+lasttitle+'</a>';
	}
	if (!docmode)
		str += '<td class="con_rel_td_star_'+contentbg+'">'
			+evaStr(star, neval)+'</td></tr>';
	else	str += '<td class="con_rel_td_star_'+contentbg+'">'
			+star+'/'+neval+'</td></tr>';
	
	contentbg = tmpcontentbg;
	docliststr += str;

	if (doccurrmin < 0)
		doccurrmin = num;
	doccurrmin = doccurrmin <= num ? doccurrmin : num;
	doccurrmax = doccurrmax >= num ? doccurrmax : num;
}

function replaceStrings(from, to, str) 
{
	var i, len = from.length;
	for(i=0;i<len;i++)
		str= replaceString(from[i], to[i], str);
	return str;
}

function con_zoom(ftime, size) {
	var obj = document.getElementById('con_'+ftime+'_content_box');

	if (obj) {
		obj.style.fontSize = size + 'px';
		obj.style.lineHeight = (size * 1.5) + 'px';
	}
}

function con_reply_box_init(ftime, contentbg, inndboard, outgoing, anony, 
	bnum, num, thread) {
	var obj = document.getElementById('con_'+ftime+'_reply');
	var obj2 = document.getElementById('con_reply_textarea');
	var str, tmpobj, i;
	var title;

	if (!obj)
		return;

	if (obj2 && obj.innerHTML == '') {
		if (confirm('�Ƿ�ر���һ�ظ����ڣ�')) {
			for (i = 0; i < conlist.length; i++) {
				tmpobj = document.getElementById(
					'con_'+conlist[i]+'_reply');
				if (tmpobj) {
					tmpobj.innerHTML = '';
					tmpobj.style.display = 'none';
				}
			}
		} else {
			return;
		}
	}

	for (i = 0; i < titlelist.length; i++) {
		if (titlelist[i].ftime == ftime) {
			title = titlelist[i].title;
			break;
		} else {
			title = '';
		}
	}
	if (title.substring(0, 4) != 'Re: ')
		title = 'Re: ' + title;

	str = '<div id="con_reply_padding" \
		class="con_reply_padding_'+contentbg+'">\
		<div id="con_reply_help">\
		���ٻظ�����[<a href="'+baseurl+'bbspst?B='+bnum+'&F=M.'
		+ftime+'.A&num='+num+(inndboard ? '&la=1' : '')+'"\
		name="�����༭����">���������������༭����</a>]\
		</div>\
		<div id="con_reply_ajax">\
		<div id="con_reply_ajax_login"></div>\
		<div class="clear"></div>\
		<div id="con_reply_ajax_msg">&nbsp;</div>\
		</div>\
		<form id="con_reply_form" action="'+baseurl+'bbssnd" method="post">\
		<div id="con_reply_head_box">\
		<div id="con_reply_flag_box">\
		[TeX��ʽ <input type="checkbox" \
		class="con_reply_checkbox" name="usemath">]';
	if (inndboard) {
		str += ' [ת�� <input type="checkbox" \
		class="con_reply_checkbox" name="outgoing"';
		if (outgoing)
			str += ' checked';
		str += '>]';
	}
	if (anony) {
		str += ' [���� <input type="checkbox" \
		class="con_reply_checkbox" name="anony"';
		if (anony == 2)
			str += 'checked';
		str += '>]';
	}
	str +=	'</div>\
		<div id="con_reply_title_help">ʹ�ñ��⣺</div>\
		<input id="con_reply_title" \
		type="text" maxlength="40">\
		</div><div class="clear"></div>\
		<div id="con_reply_main">\
		<textarea id="con_reply_textarea" onKeyDown=\
		"con_keydown(event, 2);"></textarea>\
		</div>\
		<div id="con_reply_nav">\
		<div id="con_reply_nav_left">\
		<input id="con_reply_submit" type="button" value="����" \
		onclick="con_post_reply();">\
		</div>\
		<div id="con_reply_nav_right">\
		<input id="con_reply_reset" type="button" value="���" \
		onclick="con_clear_reply(1);">\
		</div>\
		</div><div class="clear"></div>\
		<input type="hidden" name="B" value="'+bnum+'">\
		<input type="hidden" name="th" value="'+thread+'">\
		<input type="hidden" name="ref" value="M.'+ftime+'.A">\
		<input type="hidden" name="rid" value="'+num+'">\
		<input type="hidden" name="ajax" id="submit_ajax" value="1">\
		<input type="hidden" name="title" id="submit_title" value="">\
		<input type="hidden" name="text" id="submit_text" value="">\
		</form></div>';
	
	obj.style.display = '';
	obj.innerHTML = str;
	document.getElementById('con_reply_title').style.width = 
		(document.getElementById('con_reply_head_box').offsetWidth - 
		document.getElementById('con_reply_flag_box').offsetWidth - 
		document.getElementById('con_reply_title_help').offsetWidth) - 
		20 + 'px';
	document.getElementById('con_reply_textarea').style.width = 
		document.getElementById('con_reply_padding').offsetWidth -
		10 + 'px';
		/* ̫���� */
	document.getElementById('con_reply_textarea').focus();
	document.getElementById('con_reply_title').value = title;
	con_ajax_test_login();
}

function con_ajax_test_login() {
	var obj1 = document.getElementById('con_reply_ajax_login');
	var obj2 = document.getElementById('con_reply_ajax_msg');
	var str;

	if (!obj1 || !obj2)
		return;

	ajax_test_login();

	if (ajax_userid != 'guest' && ajax_userid != '') {
		obj2.innerHTML = '';
		return;
	}

	str = '<div id="con_reply_ajax_login_left"> \
		�û�����<input id="con_reply_ajax_login_username" \
		type="text" maxlength="14" class="ipnutbox">����\
		���룺<input id="con_reply_ajax_login_password" \
		type="password" maxlength="16" class="inputbox" \
		onKeyDown=\"con_keydown(event, 1);\">\
		</div><div id="con_reply_ajax_login_right"\
		><a href="/ipmask.html" name="�˽�IP��֤��Χ"\
		>IP��Χ</a>��[<input type="radio" name="iprange" value="1" \
		class="con_reply_radio" id="con_reply_iprange_1" checked> 1] ';
	if (!narrow)
		str += '[<input type="radio" name="iprange"\
		class="con_reply_radio" \
		id="con_reply_iprange_2" value="2"> 2] ';
	str += '[<input type="radio" name="iprange" \
		class="con_reply_radio" \
		id="con_reply_iprange_8" value="8"> 8] ';
	if (!narrow)
		str += '[<input type="radio" name="iprange" \
		class="con_reply_radio" \
		id="con_reply_iprange_32" value="32"> 32] ';
	str += '[<input type="radio" name="iprange" \
		class="con_reply_radio" \
		id="con_reply_iprange_256" value="256"> 256]\
		</div>';
	
	obj1.innerHTML = str;
}

function con_clear_reply(conf) {
	var obj = document.getElementById('con_reply_form');

	if (!obj)
		return;
	
	if (!conf) {
		obj.reset();
		return;
	}
	
	if (confirm('���ɾ���ѱ༭�õ�������'))
		obj.reset();
}

function con_post_reply() {
	var objlist = new Array('con_reply_textarea', 
		'con_reply_iprange_1', 
		'con_reply_iprange_2',
		'con_reply_iprange_8',
		'con_reply_iprange_32',
		'con_reply_iprange_256',
		'con_reply_reset', 
		'con_reply_submit');
	var obj = document.getElementById('con_reply_textarea');
	var div = document.getElementById('con_reply_ajax_msg');
	var form = document.getElementById('con_reply_form');
	var logined = 0;
	var str, ipmask = -1, tmpobj;

	function switch_status(flag) {
		for (i = 0; i < objlist.length; i++) {
			tmpobj = document.getElementById(objlist[i]);
			if (tmpobj)
				tmpobj.disabled = flag;
		}
		tmpobj = document.getElementById(
			'con_reply_ajax_login_password');

		if (tmpobj)
			tmpobj.disabled = flag;
		tmpobj = document.getElementById(
			'con_reply_ajax_login_username');
		if (tmpobj)
			tmpobj.disabled = flag;
	}

	function fail_return() {
		switch_status(false);
		div.innerHTML = '';
	}

	for (i = 3; i < 7; i++) {
		tmpobj = document.getElementById(objlist[i]);
		if (tmpobj) {
			if (tmpobj.checked)
				ipmask = tmpobj.value;
				break;
		}
	}
	if (ipmask < 0)
		ipmask = 0;

	if (!div || !form)  
		return;

	if (obj.value == '' || /^\s*$/.test(obj.value)) {
		alert('���ܷ�������¡�');
		return;
	}
	
	switch_status(true);

	if (ajax_userid == 'guest') {
		var obj1 = document.getElementById(
			'con_reply_ajax_login_username');
		var obj2 = document.getElementById(
			'con_reply_ajax_login_password');
		var ipmask;

		if (!obj1 || !obj2) {
			str = '�����ڲ�����������˵���¼�������ԡ�';
			div.innerHTML = str;
			setTimeout(fail_return, 5000);
			return;
		}

		if (obj1.value == '' || obj2.value == '') {
			alert('���ȵ�¼');
			setTimeout(fail_return, 5000);
			return;
		}
		ajax_do_login(div, obj1, obj2, ipmask);
		session = smagic.substring(defaultsmagic.length, 
			smagic.length);
		document.cookie = 'SESSION=' + session + '; anonypid = ' + anonypid + '; path = /;';
		logined = 1;
	}

	if (ajax_userid == 'guest') {
		setTimeout(fail_return, 5000);
		return;
	}

	div.innerHTML = '��¼�ɹ������ڷ������¡�';

	if (logined) {
		form.action = '/' + smagic + '/bbssnd?';
		
	}

	if (self.location != top.location && logined) {
		parent.f2.window.location.href = '/' + defaultsmagic + '/bbsleft';
		parent.f4.window.location.href = '/' + defaultsmagic + '/bbsfoot';
	}

	document.getElementById('submit_text').value = obj.value;
	document.getElementById('submit_title').value = 
		document.getElementById('con_reply_title').value;
	form.submit();
}

function con_docnav_init (ftime, offset, nowpage, totalpage, mode) {
	var obj = document.getElementById('con_doclist_nav_padding');
	var obj2 = document.getElementById('con_doclist_ajax_padding');
	var str;

	if (!obj || !obj2)
		return;

	docmode = mode;

	str = '<div id="con_doclist_nav_left">\
		<input type="button" value="��һҳ" \
		id="con_doclist_button_left" \
		onclick="con_doclist_update(-1, '+ftime+', '
		+offset+', -1);">��\
		<input type="button" value="��ҳ" \
		id="con_doclist_button_first" \
		onclick="con_doclist_update(-1, '+ftime+', '
		+offset+', -2);"></div>';
	str += '<div id="con_doclist_nav_right">\
		<input type="button" value="ĩҳ" \
		id="con_doclist_button_last" \
		onclick="con_doclist_update(-1, '+ftime+', '
		+offset+', 2);">��\
		<input type="button" value="��һҳ" \
		id="con_doclist_button_right" \
		onclick="con_doclist_update(-1, '+ftime+', '
		+offset+', 1);"></div>';
	if (!docmode)
		str += '���������б�';
	else
		str += '���������б�';
	if (nowpage > 0 && totalpage > 0)
		str += '[��'+nowpage+'/'+totalpage+'ҳ]';
	
	obj.innerHTML = str;
	obj2.innerHTML = '&nbsp;';

}

function con_doclist_update(num, ftime, offset, direction) {
	var obj1 = document.getElementById('con_doclist_button_left');
	var obj2 = document.getElementById('con_doclist_button_right');
	var obj3 = document.getElementById('con_doclist_button_first');
	var obj4 = document.getElementById('con_doclist_button_last');
	var div = document.getElementById('con_doclist_ajax_padding');
	var tag = document.getElementById('con_doclist_content');
	var poststr, url, errorstr;
	var start = doclistcurrentnum - offset + 1;

	function con_clean_ajax_msg() {
		div.innerHTML = '&nbsp;';
		obj1.disabled = false;
		obj2.disabled = false;
		obj3.disabled = false;
		obj4.disabled = false;
	}

	errorstr = '����<a href="'+baseurl+'doc?B='+board+'" \
		name="���ر�������">����</a\
		>�����������б�鿴�������¡�';

	if (!obj1 || !obj2 || !obj3 || !obj4 ||!div || !tag)
		return;

	obj1.disabled = true;
	obj2.disabled = true;
	obj3.disabled = true;
	obj4.disabled = true;

	if (!http)
		ajax_getHTTPObject();
	if (!http) {
		div.innerHTML = '����ajax����ʧ�ܡ�' + errorstr;
		return;
	}

	if (docmode) {
		if (direction == 1)
			start = doccurrmax;
		else
			start = doccurrmin - 2; 
		if (docmode == 2) {
			start = num;
			docmode = 1;
		}
	}

	url = baseurl + 'doc?ajax=con';
	poststr = 'B='+board+'&s='+start+'&f='
		+ftime+'&o='+offset+'&d='+direction+'&m='+docmode;
	http.open('POST', url, true);
	http.setRequestHeader('Content-Type', 
		'application/x-www-form-urlencoded');
	http.onreadystatechange = function () {
		switch (http.readyState) {
			case 0:
				div.innerHTML = '���ڳ�ʼ��ajax����...';
				break;
			case 1:
				div.innerHTML = '��������ajax����...';
				break;
			case 2:
				div.innerHTML = '����ajax���̳ɹ�...';
				break;
			case 3:
				div.innerHTML = '�ȴ���������Ӧ...';
				break;
			case 4:
				if (http.status != 200 || 
					http.responseText.indexOf('con_') 
						!= 0) {
					div.innerHTML = '������Ӧ��ʧ��(\
						ԭ��'+http.responseText
						+')��' + errorstr;
					obj1.disabled = false;
					obj2.disabled = false;
				} else {
					div.innerHTML = '���³ɹ���';
					eval(http.responseText);
					setTimeout(con_clean_ajax_msg, 2000);
				}
		}
	}
	http.send(poststr);
}

function con_doclist_head(objname) {
	var obj = document.getElementById(objname);

	if (!obj)
		return;
	obj.innerHTML = '';
	docliststr = '';
	doclistcurrentnum = -1;

	docliststr = '<table id="con_rel_table" class="con_rel_table"> \
		<tr class="con_rel_tr_head">\
		<td class="con_rel_td_num_head">���</td>\
		<td class="con_rel_td_flag_head">״̬</td>\
		<td class="con_rel_td_owner_head">����</td>\
		<td class="con_rel_td_date_head">����</td>\
		<td class="con_rel_td_title_head">����</td>';
	if (!docmode)
		docliststr += '<td class="con_rel_td_star_head">�Ǽ�</td>';
	else
		docliststr += '<td class="con_rel_td_star_head"\
			>�ظ�</td>';
	docliststr += '</tr>';
	doccurrmin = -1;
	doccurrmax = 0;
}

function con_doclist_end(objname) {
	var obj = document.getElementById(objname);
	
	if (!obj)
		return;
	
	docliststr += '</table>';

	obj.innerHTML = docliststr;
}

function con_check_reply() {
	var obj = document.getElementById('con_reply_title');

	if (!obj || obj.value == '' || /^\s*$/.test(obj.value))
		return true;

	return;
}

function con_keydown(event, type) {
	var obj;

	if (type == 1) {
		obj = document.getElementById('con_reply_textarea');
	} else if (type == 2) {
		obj = document.getElementById('con_reply_submit');
	} else
		return;

	if (!obj) {
		return;
	}
	
	if (event.ctrlKey && event.keyCode == 13 && type == 2) {
		obj.click();
		event.returnValue = false;	//for ie use
		event.preventDefault();		//for ff use
		return 0;
	} else if (event.keyCode == 13 && type == 1) {
		obj.focus();
		event.returnValue = false;	//for ie use
		event.preventDefault();		//for ff use
	}
	return;
}

function con_show_load(ftime, currsize, totalsize) {
	obj = document.getElementById('con_' + ftime + '_content_msg');

	if (!obj)
		return;
	
	obj.innerHTML = '���ڶ�ȡ������Ϣ��<br>['
		+parseInt(currsize / totalsize * 100)+
		'%��(�Ѷ�ȡ '+currsize+' �ֽ� / �� '+totalsize+
		'�ֽ�)]';
}

function con_hide_load(ftime, type) {
	obj = document.getElementById('con_' + ftime + '_content_msg');

	if (!obj)
		return;

	function con_hide_load_clear() {
		obj.innerHTML = '';
		return;
	}	

	if (!type)
		setTimeout(con_hide_load_clear, 1000);
	else
		con_hide_load_clear();
}

function con_new_title(ftime, title) {
	var i;

	function con_new_title_item(ftime, title) {
		this.ftime = ftime;
		this.title = title;
		return;
	}
	for (i = 0; i < titlelist.length; i++) {
		if (titlelist[i].ftime == ftime)
			return;
	}

	titlelist[titlelist.length] = new con_new_title_item(ftime, title);
}

function con_resize(object) {
	var box = document.getElementById('con_main_left');

	if (!box)
		return;
	if (object.offsetWidth + 30 > box.offsetWidth)
		object.style.width = box.offsetWidth - 30 + 'px';
	return;
}
