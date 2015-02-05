#ifndef _CALLBACK_H
#define _CALLBACK_H

void _OnStartDrag(CMouseEvent * evt)
{
	evt->target->stage->startDrag();
}



void OnBtnOKClick(CMouseEvent * evt)
{
	CHECK_LEFT_BUTTON
	ExitProcess(0);
}
#endif