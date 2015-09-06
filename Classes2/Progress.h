#ifndef __PREGRESS_H__
#define __PREGRESS_H__
#include "cocos2d.h"
USING_NS_CC;

class Progress :public Sprite
{
public:
	Progress();
	~Progress();
	bool init(const char* background, const char* filename);

	static Progress * create(const char* background, const char* fill);

	void setFill(ProgressTimer * fill ){_fill = fill;}
	void setProgress(float percentage){_fill->setPercentage(percentage);}
private:
	ProgressTimer* _fill;
};



#endif





