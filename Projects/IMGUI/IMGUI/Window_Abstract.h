#pragma once
#include <string>

class Window_Abstract
{
public:
	Window_Abstract();
	~Window_Abstract();

	virtual void draw() = 0;
	virtual const std::string windowTitle() const = 0;

	/* We're fine with this kind of access: no one other than the UI thread will need to use this. */
	bool & visible() { return m_visible; };
	void setVisible(bool visible) { m_visible = visible; }
private:
	bool m_visible;
};

