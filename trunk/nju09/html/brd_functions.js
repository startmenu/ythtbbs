var FOLDER_NUM = 16;
var FOLDER_BRD_NUM = 32;
var FAVOR_BRD_NUM = 99;
var DEFAULT_BRD = "BBSHelp";
var currFolder = 0;
var errorMsg = '';
var saveMsg = '';
var displayBrdBySec = true;	// True: Display boards by sec;
				// False: Display boards alphabetically.
var displayBrdTitle = false;	// True: Show board Title (Eng Name);
				// False: Show board Name (Chn Name).

var Favor;
var secTree;
var boardList;


/* Basic Opearations on Favor Folders */
function inCurrFolder(bname, currFolder) {
	var i;
	var tmpBoardNum;
	var tmpBoardIdx;

	if (currFolder < 0 || currFolder > FOLDER_NUM - 1)
		return false;
	tmpBoardNum = Favor.folder[currFolder].boardNum;
	for (i = 0; i < tmpBoardNum; i++) {
		tmpBoardIdx = Favor.folder[currFolder].bidx[i];
		if (bname.toLowerCase() == 
				Favor.boards[tmpBoardIdx].toLowerCase())
			return i;
	}
	return false;
}

function inBoardList(bname) {
	var i;

	for (i = 1; i < FAVOR_BRD_NUM + 1; i++) {
		if (bname.toLowerCase() == Favor.boards[i].toLowerCase())
			return i;
	}
	return false;
}

function _addBoard(bname, tagFolder) {
	var tmpBoardNum;
	var tmpBoardIdx;
	var i;

	if (tagFolder < 0 || tagFolder > FOLDER_NUM - 1) {
		errorMsg = '�ƺ�û�����Ŀ¼��������';
		return false;
	}
	tmpBoardNum = Favor.folder[tagFolder].boardNum;

	if (tmpBoardNum < 0 || tmpBoardNum > FOLDER_BRD_NUM - 1) {
		errorMsg = '��ǰĿ¼���ư������Ѵﵽ����(' + FOLDER_BRD_NUM;
		errorMsg += ')��';
		return false;
	}

	if (!((tmpBoardIdx = inBoardList(bname)) === false)) {
		Favor.folder[tagFolder].bidx[tmpBoardNum] = tmpBoardIdx;
		Favor.folder[tagFolder].boardNum++;
		return true;
	}

	if (Favor.boardNum == FAVOR_BRD_NUM) {
		errorMsg = '��ǰ���ĵ������������Ѿ��ﵽ����(';
		errorMsg += FAVOR_BRD_NUM + ')��';
		return false;
	}

	for (i = 1; i < FAVOR_BRD_NUM + 1; i++) {
		if (Favor.boards[i].length == 0) {
			Favor.boards[i] = bname;
			Favor.folder[tagFolder].bidx[tmpBoardNum] = i;
			Favor.folder[tagFolder].boardNum++;
			Favor.boardNum++;
			return true;
		}
	}
	errorMsg = 'δ֪�����볢��ˢ�±�ҳ�档';
	return false;
}

function addFolder() {
	var foldername = document.getElementById('newfoldername').value;
	var desc = document.getElementById('newfolderdesc').value;

	if (_addFolder(foldername, desc)) {
		alert("Ŀ¼��ӳɹ�����ע�⼰ʱ�������ݡ�");
		displayBrdAll();
		return true;
	}
	alert(errorMsg);
	return false;
}

function _addFolder(foldername, desc) {
	var tmpFolderNum;
	var tmpBoardNum;
	var tmpBoardIdx;
	var retv;

	tmpFolderNum = Favor.folderNum;
	if (tmpFolderNum <0 || tmpFolderNum > FOLDER_NUM - 2) {
		errorMsg = '��ǰĿ¼���Ѿ��ﵽ����(' + FOLDER_NUM + ')��';
		return false;
	}

	if (foldername.length == 0) {
		errorMsg = '������һ��Ŀ¼�����֡�';
		return false;
	}

	Favor.folder[tmpFolderNum + 1].name = foldername;
	Favor.folder[tmpFolderNum + 1].desc = desc;
	Favor.folderNum++;

	if ((tmpBoardIdx = inBoardList(DEFAULT_BRD)) === false) {
		if (Favor.boardNum == FAVOR_BRD_NUM) {
			retv = _addBoard(Favor.boards[1], tmpFolderNum + 1);
		} else {
			retv = _addBoard(DEFAULT_BRD, tmpFolderNum + 1);
		}
	} else {
		retv = _addBoard(DEFAULT_BRD, tmpFolderNum + 1);
	}

	if (!retv) {
		Favor.folder[tmpFolderNum + 1].name = '';
		Favor.folder[tmpFolderNum + 1].desc = '';
		Favor.folderNum--;
		return false;
	}
	currFolder = tmpFolderNum + 1;
	return true;
}

function _delBoard(bname, tagFolder, keeplast) {
	var tmpBoardPos;
	var tmpBoardIdx;
	var i;

	if (tagFolder < 0 || tagFolder > FOLDER_NUM - 1) {
		errorMsg = 'û�����Ŀ¼��';
		return false;
	}

	tmpBoardPos = inCurrFolder(bname, tagFolder);
	if (tmpBoardPos === false)
		return true;

	if (Favor.folder[tagFolder].boardNum == 1 && 
			(keeplast || tagFolder == 0)) {
		errorMsg = '���Ŀ¼����Ҫ��һ�����档';
		return false;
	}

	tmpBoardIdx = Favor.folder[tagFolder].bidx[tmpBoardPos];
	for (i = tmpBoardPos; i < FOLDER_BRD_NUM - 1; i++) {
		Favor.folder[tagFolder].bidx[i] = 
			Favor.folder[tagFolder].bidx[i + 1];
	}
	Favor.folder[tagFolder].bidx[FOLDER_BRD_NUM - 1] = 0;
	Favor.folder[tagFolder].boardNum--;

	for (i = 0; i < FOLDER_NUM; i++) {
		if (!(inCurrFolder(bname, i) === false))
			return true;
	}

	Favor.boards[tmpBoardIdx] = '';
	Favor.boardNum--;
	return true;
}

function delFolder() {
	var msg = '';
		
	msg += '���Ҫɾ�����Ŀ¼��';
	msg += '����ֻ�������Ŀ¼�İ��潫ͬʱ��ɾ��';
	if (!confirm(msg))
		return;
	if (_delFolder(currFolder)) {
		currFolder = 0;
		displayBrdAll();
		alert("Ŀ¼�ѳɹ�ɾ������ע�⼰ʱ�����µ����ݡ�");
		return true
	}
	alert(errorMsg);
	return false;
}

function _delFolder(tagFolder) {
	var tmpFolderBoardNum;
	var tmpBoardName;
	var i;
	
	if (tagFolder < 0 || tagFolder > FOLDER_NUM - 1) {
		errorMsg = "û�����Ŀ¼��";
		return false;
	} else if (tagFolder == 0) {
		errorMsg = "����ɾ����Ŀ¼��";
		return false;
	}

	tmpFolderBoardNum = Favor.folder[tagFolder].boardNum;
	for (i = tmpFolderBoardNum - 1; i >= 0; i--) {
		tmpBoardName = Favor.boards[Favor.folder[tagFolder].bidx[i]];
		if (!_delBoard(tmpBoardName, tagFolder, 0))
			return false;
	}
	
	for (i = tagFolder; i < FOLDER_NUM - 1; i++) {
		Favor.folder[i] = Favor.folder[i + 1];
	}
	Favor.folder[FOLDER_NUM - 1].name = '';
	Favor.folder[FOLDER_NUM - 1].desc = '';
	Favor.folder[FOLDER_NUM - 1].boardNum = 0;
	for (i = 0; i < FOLDER_BRD_NUM; i++) {
		Favor.folder[FOLDER_NUM - 1].bidx[i] = 0;
	}
	Favor.folderNum--;
	return true;
}

function editFolder(currFolder) {
	var foldername = document.getElementById('foldername').value;
	var desc = document.getElementById('folderdesc').value;

	if (_editFolder(foldername, desc, currFolder)) {
		displayBrdLeft1();
		alert("�޸ĳɹ�����ע�⼰ʱ�������ݡ�");
		return true;
	}
	alert(errorMsg);
	return false;
}

function _editFolder(foldername, desc, currFolder) {
	
	if (currFolder < 0 || currFolder > FOLDER_NUM - 1 || 
			Favor.folder[currFolder].boardNum == 0) {
		errorMsg = "û�����Ŀ¼��";
		return false;
	} else if (currFolder == 0) {
		errorMsg = "������������Ŀ¼��";
		return false;
	} else if (foldername.length == 0) {
		errorMsg = "������Ŀ¼�����֡�";
		return false;
	}

	Favor.folder[currFolder].name = foldername;
	Favor.folder[currFolder].desc = desc;
	return true;
}

/* Display Functions */

function singleBoard(obj) {
	if (obj.checked) {
		if (!_addBoard(obj.name, currFolder)) {
			alert(errorMsg);
			obj.checked = false;
		}
	} else {
		if (!_delBoard(obj.name, currFolder, 1)) {
			alert(errorMsg);
			obj.checked = true;
		}
	}
	displayBrdLeft2();
}

function displayBrdLeft1() {
	var retv = '';
	var obj = document.getElementById('brd_main_left_padding1');
	var i;

	if (!obj)
		return;
	for (i = 0; i < FOLDER_NUM; i++) {
		if (Favor.folder[i].boardNum == 0)
			break;
		if (i == currFolder)
			retv += '<div class="brd_folder_item_focus" ';
		else {
			retv += '<div class="brd_folder_item" ';
			retv += 'onMouseOver = "this.className = ';
			retv += '\'brd_folder_item_hover\';" ';
			retv += 'onMouseOut = "this.className = ';
			retv += '\'brd_folder_item\';" ';
			retv += 'onClick="changeCurrFolder(' + i + ');"'
		}
		retv += '>';
		retv += Favor.folder[i].name;
		retv += '</div>'
	}
	obj.innerHTML = retv;	
}

function displayBrdLeft2 () {
	var obj = document.getElementById('brd_main_left_padding2');
	var folderName;
	var folderDesc;
	var retv = '';

	if (!obj)
		return;

	retv += '<h2>�޸ĵ�ǰĿ¼</h2>';
	retv += '�����ơ�<input id="foldername" type="text" maxlength="16" ';
       	retv += 'value="';
	retv += Favor.folder[currFolder].name + '" ';
	if (currFolder == 0)
		retv += 'disabled';
	retv += '/><br />';
	retv += '����ע��<input id="folderdesc" type="text" maxlength="16" ';
	retv += 'value="';
	retv += Favor.folder[currFolder].desc + '" ';
	if (currFolder == 0)
		retv += 'disabled';
	retv += '/><br />';
	retv += '<input type="button" value="���ޡ��ġ�" ';
	retv += 'onClick = "editFolder(' + currFolder +');" ';
	if (currFolder == 0)
		retv += 'disabled';
	retv += '><br /><br />';
	retv += '<h2>ɾ����ǰĿ¼</h2>';
	retv += 'ɾ����ǰĿ¼��ȫ���������档';
	retv += '<input type="button" value="��ȷ������" ';
	retv += 'onClick="delFolder();" ';
	if (currFolder == 0)
		retv += 'disabled';
	retv += '><br /><br />';
	retv += '<h2>������Ŀ¼</h2>';
	retv += '�����ơ�<input id="newfoldername" type="text" maxlength="16" ';
	if (Favor.folderNum > FOLDER_NUM - 2)
		retv += 'disabled';
	retv += '/><br />';
	retv += '����ע��<input id="newfolderdesc" type="text" maxlength="16" ';
	if (Favor.folderNum > FOLDER_NUM - 2)
		retv += 'disabled';
	retv += '/><br />';
	retv += '<input type="button" value="����������" ';
	retv += 'onClick="addFolder()" ';
	if (Favor.folderNum > FOLDER_NUM - 2)
		retv += 'disabled';
	retv += '><br /><br />';
	retv += '<h2>�ղؼ�ʹ��״��</h2>';
	retv += '������������';
	retv += Favor.boardNum + '/' + FAVOR_BRD_NUM + '<br />';
	retv += 'Ŀ¼��������';
	retv += Favor.folderNum + 1 + '/' + FOLDER_NUM + '<br />'; 
	retv += '��ǰĿ¼����';
	retv += Favor.folder[currFolder].boardNum + '/' + FOLDER_BRD_NUM;
	retv += '<br /><br />';	
	retv += '<h2>����ȫ���޸�</h2>';
	retv += '���汾���޸ĵ����ݡ�<br />';
	retv += '<span class="red">�ڵ�������桱ǰ��</span><br />';
	retv += '<span class="red">�κ��޸Ķ����ᱻ��¼��</span><br />';
	retv += '<input type="button" value="�������桡" ';
	retv += 'onClick = "saveFavor();">';
	retv += '<br />';
	obj.innerHTML = retv;
}

function saveFavor() {
	/* Should be updated to AJAX later */
	var obj1 = document.getElementById('saveJSON');
	var obj2 = document.getElementById('saveJSONString');
	if (!obj1 || !obj2)
		return false;
	obj2.value = Favor.toJSONString();
	obj1.submit();
	return true;
}

function switchDisplayBrdBySec() {
	displayBrdBySec = !displayBrdBySec;
	displayBrdRight();
}

function switchDisplayBrdTitle() {
	displayBrdTitle = !displayBrdTitle;
	displayBrdRight();
}

function displayBrdRight() {
	var obj = document.getElementById('brd_main_right_padding');
	var retv = '';
	var i, j, secLen, boardLen, pos, secChar;
	var tmpBoardList, tmpBoardObject;

	function getBoardString(bidx) {
		var retv = '';
		if (bidx > boardList.length - 1)
			return retv;
		retv += '<td width="32%">';
		retv += '<input type="checkbox" onClick="singleBoard(this)" ';
		if (!(inCurrFolder(boardList[bidx].filename, currFolder) 
					=== false))
			retv+= 'checked';
		retv += ' name="' + boardList[bidx].filename +'">��';
		if (displayBrdTitle)
			retv += boardList[bidx].filename;
		else
			retv += boardList[bidx].title;
		retv += '</td>';
		return retv;
	}

	if (!obj)
		return;

	retv += '<table width="100%" border="0" cellpadding="2" ';
	retv += 'cellspacing="2">';
	retv += '<tr><td colspan="3" class="brd_right_head">';
	retv += '��������ʽ��';
	if (displayBrdBySec) {
		retv += '[����] ';
		retv += '[<a href="#" onClick="switchDisplayBrdBySec();">';
		retv += '<span class="blue">';
		retv += '��ĸ</span></a>]����';
	} else {
		retv += '[<a href="#" onClick="switchDisplayBrdBySec();">';
		retv += '<span class="blue">';
		retv += '����</span></a>] [��ĸ]����';
	}
	retv += '�������ơ�';
	if (displayBrdTitle) {
		retv += '[<a href="#" onClick="switchDisplayBrdTitle();">';
		retv += '<span class="blue">';
		retv += '����</span></a>] [Ӣ��]';
	} else {
		retv += '[����] ';
		retv += '[<a href="#" onClick="switchDisplayBrdTitle();">';
		retv += '<span class="blue">';
		retv += 'Ӣ��</span></a>]';
	}
	retv += '</td></tr>';
	boardList = jsonBoardList.parseJSON();
	boardLen = boardList.length;
	if (displayBrdBySec) {
		secLen = secTree.secName.length;
		for (i = 0; i < secLen; i++) {
			pos = 0;
			secChar = secTree.secChar[i];
			retv += '<tr><td colspan="3">';
			retv += '<h2>' + secTree.secName[i] + '</h2>';
			retv += '</td></tr>';
			for (j = 0; j < boardLen; j++) {
				if (boardList[j].sec1.charAt(0) != secChar &&
						boardList[j].sec2.charAt(0) != 
							secChar) {
					continue;
				}
				if (pos % 3 == 0)
					retv += '<tr>';
				retv += getBoardString(j);
				if (pos % 3 == 2)
					retv += '</tr>';
				pos++;
			}
			while (pos % 3) {
				retv += '<td>&nbsp;</td>';
				if (pos % 3 == 2)
					retv += '</tr>';
				pos++;
			}
		}
	} else {
		for (i = 0; i < boardLen; i++) {
			for (j = i + 1; j < boardLen; j++) {
				if (boardList[i].filename.toLowerCase() > 
						boardList[j].filename.
							toLowerCase()) {
					tmpBoardObject = boardList[i];
					boardList[i] = boardList[j];
					boardList[j] = tmpBoardObject;
				}
			}
		}
		pos = 0
		for (i = 0; i < boardLen; i++) {
			if (pos % 3 == 0)
				retv += '<tr>';
			retv += getBoardString(i);
			if (pos % 3 == 2)
				retv += '</tr>';
			pos++;
		}
		while (pos % 3) {
			retv += '<td>&nbsp;</td>';
			if (pos % 3 == 2)
				retv += '</tr>';
			pos++;
		}
	}
	retv += '</table>';

	obj.innerHTML = retv;
	return;
}

function changeCurrFolder(tagFolder) {
	if (tagFolder < 0 || tagFolder > Favor.folderNum || 
			currFolder == tagFolder)
		return;
	currFolder = tagFolder;
	displayBrdAll();
}
function displayBrdAll() {
	displayBrdLeft1();
	displayBrdLeft2();
	displayBrdRight();
}

function displayFavorSetup() {
	var retv;

	function resizeFavor() {
		var obj1 = document.getElementById('brd_main');
		var obj2 = document.getElementById('brd_main_right');

		if (!obj1 || !obj2)
			return;

		if (obj1.offsetWidth < 850)
			obj1.style.width = 850;

		obj2.style.width = obj1.offsetWidth - 290 + 'px';
	}

	Favor = jsonFavor.parseJSON();
	secTree = jsonSecTree.parseJSON();
	boardList = jsonBoardList.parseJSON()

	retv  = '<div id="brd_main">';
	retv += '<div id="brd_main_head"><h1>���ø����ղؼ�</h1>';
	if (saveMsg.length > 0)
		retv += saveMsg;
	retv += '</div>';
	retv += '<div class="clear"></div>';
	retv += '<div id="brd_main_left"><div id="brd_main_left1">'
	retv += '<div id="brd_main_left_padding1">';
	retv += '</div></div>';
	retv += '<div id="brd_main_left2"><div id="brd_main_left_padding2">';
	retv += 'left2</div></div></div><div id="brd_main_right">';
	retv += '<div id="brd_main_right_padding"></div></div>';
	retv += '<div class="clear"></div>';
	retv += '<div id="brd_main_foot"></div>';
	retv += '</div>';

	document.write(retv);

	resizeFavor();

	displayBrdAll();

	retv  = '<div style="display: none;">';
	retv += '<form id="saveJSON" action="bbsmybrd?action=1" method="post">';
	retv += '<textarea value="" id="saveJSONString" name="saveJSONString">';
	retv += '</textarea>';
	retv += '</form></div>';
	document.write(retv);
}

function displayFavorTree(mybrdmode) {
	var retv = '';
	var obj = document.getElementById('favorTree');

	if (!obj)
		return;

	Favor = jsonFavor.parseJSON();
	boardList = jsonBoardList.parseJSON();

	function getBoardTitle(bname) {
		var i;
		for (i = 0; i < boardList.length; i++) {
			if (boardList[i].filename.toLowerCase() == 
					bname.toLowerCase())
				return boardList[i].title;
		}
		return '';
	}
	function displayFavorBoards(Favor, tagFolder) {
		var j, bindex, retv = '';
		for (j = 0; j < FOLDER_BRD_NUM; j++) {
			bindex = Favor.folder[tagFolder].bidx[j];
			if (!bindex)
				break;
			retv += '<div id="f' + tagFolder + 'b' + j + '">';
			if (tagFolder == 0)
				retv += '��';
			else
				retv += ' ����';
			retv += '<a href="home?B=' + Favor.bNum[bindex];
			if (mybrdmode) { //Ӣ�� 
				retv += '" title="';
				retv += getBoardTitle(Favor.boards[bindex]);
				retv += '" target="_top">';
				retv += Favor.boards[bindex];
			} else {
				retv += '" title="';
				retv += Favor.boards[bindex];
				retv += '" target="_top">';
				retv += getBoardTitle(Favor.boards[bindex]);
			}
			retv += '</a></div>'
		}
		return retv;
	}

	for (i = 1; i < FOLDER_NUM; i++) {
		if (Favor.folder[i].boardNum == 0)
			break;
		retv += '<div onclick="switchFavor('+ i +');">&nbsp;&nbsp;';
	       	retv += '<a href="#here" title="' + Favor.folder[i].desc + '">';
		retv += '<img src="/folder.gif" alt="" border="0" ';
		retv += 'id="imgf' + i + '" />';
		retv += Favor.folder[i].name + '</a></div>';
		retv += '<div id="f' + i +'" style="display: none;">';
	       	retv += displayFavorBoards(Favor, i);
		retv += '</div>';
	}
	retv += '<div id="f0">' + displayFavorBoards(Favor, 0) + '</div>';
	obj.innerHTML = retv;
	return;
}

function switchFavor(currFolder) {
	var obj, i;

	if (currFolder == 0)
		return;

	obj = document.getElementById('f' + currFolder);
	if (!obj)
		return;

	if (obj.style.display == 'none') {
		obj.style.display = 'block';
		if((obj = document.getElementById('imgf' + currFolder)))
			obj.src = '/folder2.gif';
		for (i = 1; i < FOLDER_NUM; i++) {
			if (i == currFolder)
				continue;
			obj = document.getElementById('f' + i);
			if (!obj)
				break;
			if (obj.style.display != 'none') {
				obj.style.display = 'none';
				if((obj = document.getElementById(
							'imgf' + i)))
					obj.src = '/folder.gif';
			}
		}

	} else {
		obj.style.display = 'none';
		if((obj = document.getElementById('imgf' + currFolder)))
			obj.src = '/folder.gif';
	}
	return;
}

