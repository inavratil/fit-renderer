#ifndef _PASS_H_
#define _PASS_H_

class Pass
{ 

// -- Member variables

protected:

//-- Public methods 

public:
	Pass(void);
	virtual ~Pass(void);
	
	virtual void Render() = 0;

};

typedef Pass* PassPtr;

#endif

