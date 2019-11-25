#ifndef PLAYERHUD_H
#define PLAYERHUD_H

#include <QDialog>
#include "ui_playerhud.h"
#include <qmouseevent>

class MobData;
class PlayerHud : public QDialog
{
	Q_OBJECT

public:
	PlayerHud(QWidget *parent = 0);
	~PlayerHud();

	void setData(MobData *mData, float len);
	void cancelData();

private:
	Ui::PlayerHud ui;

	bool moveState;
	int	StartPosX;
	int	StartPosY;

	QRect *hpGeometry;
	QRect *energyGeometry;

protected slots:
	virtual void mousePressEvent( QMouseEvent * event );	
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
};

#endif // PLAYERHUD_H
