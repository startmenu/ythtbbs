var graph;
var secstr;
var rootstr;

function graphitem() {
	this.name = name;
	this.pos = pos;
	this.team = team;
	this.subs = 0;
	this.subgraph = new Array();
}

function freegraph(graph) {
}

function admin_print_title(sec) {
	var retv;

	retv  = '<h1>�������</h1>';
	retv += '<table><tr><td><input type="button" value="������ҳ" ';
	retv += 'onclick="location.href=\'bbsfb?A=9\'"></td>';
	retv += '<form action="bbsfb?A=9&T=7&G=' + sec + '" method="post">';
	retv += '<td><input type="submit" value="������ͼ">';
	retv += '</td></form><td><input type="button" value="��������" ';
	retv += 'onclick="bbsfb_import_teams();"></td>';
	retv += '<td><input type="button" value="���ս��" ';
	retv += 'onclick="location.href=\'bbsfb?A=9&T=9&G=' + sec + '\'"></td>';
	retv += '<td>��������</td></tr></table>';

	document.write(retv);
}

function gi(sec, root) {
	var obj = document.getElementById('bbsfb_graph_main');

	if (!obj)
		return;

	obj.innerHTML = '';
	secstr = sec;
	rootstr = root;
}

function bbsfb_admin_addgraph(pos) {
	var retv;

	retv  = '<form action="bbsfb?A=9&T=1" method="post">';
	retv += '<div align="center" class="annbox">';
	retv += '<h3>�����ӽڵ�</h3>';
	retv += '���������Ϊ��ǰ�ڵ�����һ���ӽڵ㡣<br /><br />';
	retv += '<input type="hidden" name="posstr" value="' + pos + '">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '������ڵ����ơ�<input type="text" maxlength="10" ';
	retv += 'name="graphname">����';
	retv += '<input type="button" onclick="document.getElementById(';
	retv += '\'bbsfb_admin_main\').innerHTML=\'\';" value="����">����';
	retv += '<input type="submit" value="����"><br /><br />';
	retv += '</div></form><br />';

	return retv;
}

function bbsfb_admin_delgraph(pos) {
	var retv;

	retv  = '<form action="bbsfb?A=9&T=2" method="post"';
	retv += 'onsubmit="return confirm(\'ȷ��ɾ����\')">';
	retv += '<div align="center" class="annbox">';
	retv += '<h3>ɾ���ڵ�</h3>';
	retv += '���������ɾ������ڵ㣬��ɾ����������ڵ��';
	retv += '�������ӣ�������ʹ�á�<br /><br />';
	retv += '<input type="hidden" name="posstr" value="' + pos + '">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '<input type="button" onclick="document.getElementById(';
	retv += '\'bbsfb_admin_main\').innerHTML=\'\';" value="����">����';
	retv += '<input type="submit" value="ɾ��"><br /><br />';
	retv += '</div></form><br />';

	return retv; 

}

function bbsfb_admin_renamegraph (pos) {
	var retv;

	retv  = '<form action="bbsfb?A=9&T=3" method="post">';
	retv += '<div align="center" class="annbox">';
	retv += '<h3>�������ڵ�</h3>';
	retv += '���������������������ڵ㣬������Ӱ������ڵ���ӽڵ�';
	retv += '��������鼰���̰��š�<br /><br />';
	retv += '�������µ����ơ�'
	retv += '<input type="text" name="newname" maxlength="40">����';
	retv += '<input type="hidden" name="posstr" value="' + pos + '">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '<input type="button" onclick="document.getElementById(';
	retv += '\'bbsfb_admin_main\').innerHTML=\'\';" value="����">����';
	retv += '<input type="submit" value="����"><br /><br />';
	retv += '</div></form><br />';

	return retv; 

}

function bbsfb_admin_addteam(pos) {
	var retv;

	retv  = '<form action="bbsfb?A=9&T=4" method="post">';
	retv += '<div align="center" class="annbox">';
	retv += '<h3>��������</h3>';
	retv += '����������ڵ�ǰλ������һ��ֱ�����ӣ������ǰλ�õ��ӽڵ�';
	retv += '��ֱ������֮�ʹ��ڵ���2����ô�ò�����ʧ�ܡ�<br /><br />';
	retv += '<input type="hidden" name="posstr" value="' + pos + '">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '<input type="hidden" name="tnum" value="0">';
	retv += '�������ơ�'
	retv += '<input type="text" name="tname" maxlength="15">����';
	retv += 'ѧУ���ơ�'
	retv += '<input type="text" name="tuniv" maxlength="15"><br />';
	retv += '��Աһ����'
	retv += '<input type="text" name="tmember1" maxlength="6">����';
	retv += '��Ա�ߡ���'
	retv += '<input type="text" name="tmember7" maxlength="6"><br />';
	retv += '��Ա������'
	retv += '<input type="text" name="tmember2" maxlength="6">����';
	retv += '��Ա�ˡ���'
	retv += '<input type="text" name="tmember8" maxlength="6"><br />';
	retv += '��Ա������'
	retv += '<input type="text" name="tmember3" maxlength="6">����';
	retv += '��Ա�š���'
	retv += '<input type="text" name="tmember9" maxlength="6"><br />';
	retv += '��Ա�ġ���'
	retv += '<input type="text" name="tmember4" maxlength="6">����';
	retv += '��Աʮ����'
	retv += '<input type="text" name="tmember10" maxlength="6"><br />';
	retv += '��Ա�塡��'
	retv += '<input type="text" name="tmember5" maxlength="6">����';
	retv += '��Աʮһ��'
	retv += '<input type="text" name="tmember11" maxlength="6"><br />';
	retv += '��Ա������'
	retv += '<input type="text" name="tmember6" maxlength="6">����';
	retv += '��Աʮ����'
	retv += '<input type="text" name="tmember12" maxlength="6"><br/><br/>';
	retv += '<input type="button" onclick="document.getElementById(';
	retv += '\'bbsfb_admin_main\').innerHTML=\'\';" value="����">����';
	retv += '<input type="submit" value="����"><br /><br />';
	retv += '</div></form><br />';

	return retv; 
}

function bbsfb_admin_auto(pos) {
	var retv;


	retv  = '<form action="bbsfb?A=9&T=6" method="post">';
	retv += '<div align="center" class="annbox">';
	retv += '<h3>�Զ���������</h3>';
	retv += '����������ڵ�ǰλ���Զ��������̡�<br /><br />';
	retv += '<input type="hidden" name="posstr" value="' + pos + '">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '���ն�����������<input type="text" name="result"><br />';
	retv += '��ʼ������������<input type="text" name="total"><br />';
	retv += '<input type="button" onclick="document.getElementById(';
	retv += '\'bbsfb_admin_main\').innerHTML=\'\';" value="����">����';
	retv += '<input type="submit" value="����"><br /><br />';
	retv += '</div></form><br />';

	return retv;
}

function bbsfb_admin(pos, name) {
	var obj = document.getElementById('bbsfb_admin_main');
	var retv;

	if (!obj)
		return;

	retv  = '<h2>����ڵ�</h2><br />';
	retv += '<div class="annbox" align="center">��ǰ�ڵ���[' + name + ']��';
	retv += '�ڲ���ʶ[' + pos + ']</div>';
	obj.innerHTML  = retv;
	obj.innerHTML += bbsfb_admin_addgraph(pos);
	obj.innerHTML += bbsfb_admin_delgraph(pos);
	obj.innerHTML += bbsfb_admin_renamegraph(pos);
	obj.innerHTML += bbsfb_admin_addteam(pos);
	obj.innerHTML += bbsfb_admin_auto(pos);
}

function bbsfb_admin_editteam(num, pos) {
	var retv;

	retv  = '<form action="bbsfb?A=9&T=4" method="post">';
	retv += '<div align="center" class="annbox">';
	retv += '<h3>�༭����</h3>';
	retv += '������������޸ĵ�ǰλ�õ���ӡ�<br /><br />';
	retv += '<input type="hidden" name="posstr" value="' + pos +'">';
	retv += '<input type="hidden" name="tnum" value="' + num + '">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '�������ơ�'
	retv += '<input type="text" name="tname" maxlength="15">����';
	retv += 'ѧУ���ơ�'
	retv += '<input type="text" name="tuniv" maxlength="15"><br />';
	retv += '��Աһ����'
	retv += '<input type="text" name="tmember1" maxlength="6">����';
	retv += '��Ա�ߡ���'
	retv += '<input type="text" name="tmember7" maxlength="6"><br />';
	retv += '��Ա������'
	retv += '<input type="text" name="tmember2" maxlength="6">����';
	retv += '��Ա�ˡ���'
	retv += '<input type="text" name="tmember8" maxlength="6"><br />';
	retv += '��Ա������'
	retv += '<input type="text" name="tmember3" maxlength="6">����';
	retv += '��Ա�š���'
	retv += '<input type="text" name="tmember9" maxlength="6"><br />';
	retv += '��Ա�ġ���'
	retv += '<input type="text" name="tmember4" maxlength="6">����';
	retv += '��Աʮ����'
	retv += '<input type="text" name="tmember10" maxlength="6"><br />';
	retv += '��Ա�塡��'
	retv += '<input type="text" name="tmember5" maxlength="6">����';
	retv += '��Աʮһ��'
	retv += '<input type="text" name="tmember11" maxlength="6"><br />';
	retv += '��Ա������'
	retv += '<input type="text" name="tmember6" maxlength="6">����';
	retv += '��Աʮ����'
	retv += '<input type="text" name="tmember12" maxlength="6"><br/><br/>';
	retv += '<input type="button" onclick="document.getElementById(';
	retv += '\'bbsfb_admin_main\').innerHTML=\'\';" value="����">����';
	retv += '<input type="submit" value="�޸�"><br /><br />';
	retv += '</div></form><br />';

	return retv; 
}

function bbsfb_admin_delteam(num) {
	var retv;

	retv  = '<form action="bbsfb?A=9&T=5" method="post" ';
	retv += 'onsubmit="return confirm(\'ȷ��ɾ����\');">';
	retv += '<div align="center" class="annbox">';
	retv += '<h3>ɾ��</h3>';
	retv += '���������ɾ����ǰλ�õ���ӣ�Ӧ�ú󽫲��ɻָ�';
	retv += '�����ء�<br /><br />';
	retv += '<input type="hidden" name="tnum" value="' + num + '">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '<input type="button" onclick="document.getElementById(';
	retv += '\'bbsfb_admin_main\').innerHTML=\'\';" value="����">����';
	retv += '<input type="submit" value="ɾ��"><br /><br />';
	retv += '</div></form><br />';

	return retv; 
}

function bbsfb_admin_team(name, num, pos) {
	var obj = document.getElementById('bbsfb_admin_main');
	var retv;

	if (!obj)
		return;

	retv  = '<h2>��������</h2><br />';
	retv += '<div class="annbox" align="center">��ǰ������[' + name +']';
	retv += '���ڲ����[' + num + ']��λ��[' + pos +']<div>';

	obj.innerHTML  = retv;
	obj.innerHTML += bbsfb_admin_editteam(num, pos);
	obj.innerHTML += bbsfb_admin_delteam(num);
}

function bbsfb_show_team(name, num, admin, pos, dot) {
	var obj = document.getElementById('bbsfb_graph_' +pos+ '_right_below');
	var retv;

	if (!obj)
		return;
	if (num == 0)
		return;

	retv  = '<div align="right">';
	retv += '<table cellspacing=0 cellpadding=0><tr>';
	retv += '<td id="team_' + pos + '">';
	retv += '<div class="team_text">' + name;
	if (admin) {
		retv += '<br /><br />[<a href="#" ';
		retv += 'onclick="bbsfb_admin_team(\'';
		retv += name +'\', ' + num + ', \'' + pos + '\');">����</a>]';
	}
	retv += '</div></td>';
	retv += '<td><img width="8px" src="/football/dot.gif" ';
	retv += 'border=0 height=17 /></td>';
	retv += '<td><img width=1 src="/football/dot' + dot +'.gif" ';
	if (dot) {
		retv += 'onload="this.style.height=document.getElementById(\'';
		retv += 'team_' + pos + '\').offsetHeight;">';
	} else
		retv += '>';
	retv += '</td></tr></table></div>';

	obj.innerHTML += retv;
}

function gs(admin, name, pos, status, dottype, nsub,
		team, teamname, team1, team1name, team2, team2name, score1, 
		score2) {
	var id = 'bbsfb_graph_' + pos;
	var parentid;
	var retv;
	var obj;
	var dot;

	if (status & 0x1)		//deleted
		return;
	retv  = '';
	if (pos == rootstr) {
		retv += '<h2>������ʽͼ</h2>';
		retv += '<div align=left>';
	} else 
		retv += '<div align=left>';
	retv += '<table cellpadding=0 cellspacing=0><tr>';
	retv += '<td id="' + id + '_right" class="graph_right">';
	retv += '<div id="' + id + '_right_up"></div>';
	retv += '<div id="' + id + '_right_below"></div></td>';
	retv += '<td><img width="8px" src="/football/dot.gif" ';
	retv += 'border=0 height=17 /></td>';
	retv += '<td id="' + id + '_left" class="graph_left">';
	retv += '<div class="graph_left_text"><div class="graph_left_text2">'
	retv += '<span class="block">';
	if (team)
		retv += teamname + '<br />' + '[' + score1 + ':' + score2 +']';
	else if (name) {
		retv += name;
	} else
		retv += '<img src="/football/unknown.gif">';
	retv += '</span>';
	if (admin && !/^[A-Z].*$/.test(secstr) && pos != '') {
		retv += '<div align=center><br />';
		retv += '<a href="bbsfb?A=9&G=' + pos[0] + '">';
		retv += '[����]</a></div>';
	} else if (admin) {
		retv += '<div align=center><br />';
		retv += '<a href="#" onclick="bbsfb_admin(\'';
		retv += pos +'\', \'' + name +'\');">[����]</a></div>';
	}
	retv += '</div></div></td>';
	if (pos != rootstr) {
		retv += '<td><img width="8px" src="/football/dot.gif" ';
		retv += 'border=0 height=17 /></td>';
	}
	retv += '<td id="' + id +'"><img width=1 src="';
	retv += '/football/dot' + dottype + '.gif" ';
	if (dottype) {
		retv += 'onload="this.style.height=';
		retv += 'document.getElementById(\'' + id + '\').offsetHeight"';
	} else
		retv += '/></td>';
	retv += '</tr></table>';
	retv += '</div>';

	if (pos == '') {
		parentid  = 'bbsfb_graph_main';
	} else {
		parentid  = 'bbsfb_graph_' + pos.substring(0, pos.length - 1);
		parentid += '_right_up';
	}


	obj = document.getElementById(parentid);
	if (!obj)
		return;
	obj.innerHTML += retv;
	if (nsub && team2)
		dot = 2;
	else if (!nsub && team2)
		dot = 1;
	else if (!nsub && !team2)
		dot = 0;
	else if (nsub && !team2)
		dot = 3;
	
	bbsfb_show_team(team1name, team1, admin, pos, dot);
	if (team1 || nsub)
		dot = 3;
	else
		dot = 0;
	bbsfb_show_team(team2name, team2, admin, pos, dot);
	return;
}

function change_graph(sec) {
	var obj = document.getElementById('bbsfb_graph_box');

	if (!obj)
		return;

	obj.style.visibility = 'hidden';
	obj.src = 'bbsfb?A=1&G=' + sec;

	return;
}

function bbsfb_index() {
	return;
}

function bbsfb_adjust_frame() {
	var obj = document.getElementById('bbsfb_graph_box');

	if (!obj)
		return;
	obj.style.width = '100%';
	if (obj.contentDocument) {
		obj.style.height = obj.contentDocument.body.offsetHeight + 50
					+ 'px';
		obj.style.width = '100%';
	} else if (obj.Document) {
		obj.style.height = document.body.offsetHeight + 'px';
	}
	obj.style.visibility = '';
}

function bbsfb_import_teams() {
	var obj = document.getElementById('bbsfb_admin_main');
	var retv;

	if (!obj)
		return;

	retv  = '<h3>��������</h3>';
	retv += '<form action="bbsfb?A=9&T=8" method="post" ';
	retv += 'id="bbsfb_import_form" onsubmit="bbsfb_import_filter();">';
	retv += '<p>��������ʼ������ڲ���� ';
	retv += '<input name="start" maxlength=10></p>';
	retv += '<input name="content" type="hidden">';
	retv += '<input name="G" type="hidden" value="' + secstr +'">';
	retv += '<textarea name="import_textarea" id="bbsfb_import_textarea" ';
	retv += 'style="width: 100%; height: 400px;"></textarea>';
	retv += '<p align=right><input type="submit" value="�ύ">';
	retv += '</p></form>';

	obj.innerHTML = retv;
}

function bbsfb_import_filter() {
	var obj = document.getElementById('bbsfb_import_form');

	if (!obj) 
		return;
	
	obj.content.value = obj.import_textarea.value.replace(/\r\n/gi, '\n');

	return;
}

function ri(sec, name) {
	var retv = '';
	admin_print_title(sec);

	retv += '<br /><br /><h2>���� ' + name + ' ����ս��</h2>';

	retv += '<table width="100%" cellspacing=2 cellpadding=2 border=0>';
	retv += '<form method="post" action="bbsfb?A=9&T=10&G=' + sec + '">';
	retv += '<tr style="background-color: #eee; text-align: center; ';
	retv += 'font-weight: bold; height: 21px;"';
	retv += '<td>ǿ�н���</td><td>����һ</td><td>ǿ�н���</td>';
	retv += '<td>�����</td><td>�ȷ�</td></tr>';

	document.write(retv);
}

function rf(num, posstr, team1, team1name, team2, team2name) {
	var retv = '';

	retv += '<tr style="background-color: ';
	retv += (num % 2 ? '#eee;' : 'efe');
	retv += '; text-align: center">';
	retv += '<td><input name="force_' + num + '" type="radio" ';
	retv += 'value="' + team1 + '"></td>';
	retv += '<td>' + team1name + '</td>';
	retv += '<td><input name="force_' + num + '" type="radio" ';
	retv += 'value="' + team2 + '"></td>';
	retv += '<td>' + team2name + '</td>';
	retv += '</td><td><input name="score1_' + num + '" maxlengh=2 type=';
	retv += '"text" style="width: 25px"> - <input name="score2_' + num;
	retv += '" maxlengh=2 text="text" style="width: 25px"></td></tr>'
	retv += '<input type="hidden" name="team1_' + num + '" ';
	retv += 'value="' + team1 + '">';
	retv += '<input type="hidden" name="team2_' + num + '" ';
	retv += 'value="' + team2 + '">';
	retv += '<input type="hidden" name="posstr_' + num +'" ';
	retv += 'value="' + posstr + '">';

	document.write(retv);
}

function re(num) {
	var retv = '';

	retv += '<input name="total" type="hidden" value="' + num +'"';
	retv += '<tr style="text-align: center;"><td colspan=4></td><td><br />';
	retv += '<input type="reset" value="���"> ����';
	retv += '<input type="submit" value="����"></td></tr>';
	retv += '</table>';

	document.write(retv);
}
