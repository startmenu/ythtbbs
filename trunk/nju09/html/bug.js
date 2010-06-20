bug_change_reasons = new Array(
	new Array(
		new Array(new Array("�½�"), 
			  null, 
			  new Array("���ܽ������")),
		new Array(null, null, null),
		new Array(null, null, null)
	),
	new Array(
		new Array(new Array("�½�"), 
			  new Array("���޸�", 
				    "������Ϊ�������",
				    "�ظ��ύ�˴���",
                                    "�޷����ִ˴���"),
                          null),
                new Array(new Array("���¼���", "����������������", "������޸�"),
                          null,
                          null),
		new Array(null, null, null)
	)
);	

function get_index(str)
{
	if(str == "��Ծ")
		return 0;
	if(str == "���")
		return 1;
	if(str == "�ر�")
		return 2;
	return 0;
}

function fill_owner(selectCtrl, ps, ns, owner, reporter) {
	var i, prev, next;
	var def;
	if(ps == "��Ծ" && ns == "���")
		def = reporter;
	else
		def = owner;

	for(i = 0; i < selectCtrl.options.length; i++){
		if( selectCtrl.options[i].value != def )
				selectCtrl.options[i].selected = false;
		else
				selectCtrl.options[i].selected = true;
	}
}

function fill_reason(selectCtrl, type, ps, ns, last_reason) {
	var i, prev, next;
	for (i = selectCtrl.options.length; i >= 0; i--) {
		selectCtrl.options[i] = null; 
	}
	prev =  get_index(ps);
	next =  get_index(ns);
	if(type == 1 && prev == 1 && next == 2){
		if( last_reason == "���޸�" ){
			selectCtrl.options[0] = new Option("ȷ���޸�");
		}
		else {
			selectCtrl.options[0] = new Option(last_reason);
		}
			
	} else {
		for (i = 0; i < bug_change_reasons[type][prev][next].length; i++) {
			selectCtrl.options[i] = 
				new Option(bug_change_reasons[type][prev][next][i]);
			selectCtrl.options[i].value = 
				bug_change_reasons[type][prev][next][i];
		}
	}
	selectCtrl.options[0].selected = true;
}
