#pragma once
#include <queue>

class Mouse
{
	friend class Window;
public:///Event类
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Enter,//在窗口内
			Leave,//离开窗口 
			Move,
			Invalid
		};
	private:
		/*鼠标状态*/
		Type type;
		/*事件发生时的鼠标键位的状态*/
		bool leftIsPressed;
		bool rightIsPressed;
		/*事件发生时光标的坐标*/
		int x;
		int y;
	public:
		Event() noexcept
			:
			type(Type::Invalid),
			leftIsPressed(false),
			rightIsPressed(false),
			x(0),
			y(0)
		{}
		Event(Type type, const Mouse& parent) noexcept
			:
			type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			x(parent.x),
			y(parent.y)
		{}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return{ x,y };
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool LeftIsPressed() const noexcept
		{
			return leftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return rightIsPressed;
		}
	};

/********************************************************/

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Flush() noexcept;
private:
	void OnMouseMove(int newx, int newy) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	/*计算滑轮的旋转值,若参数delta值足够大,会导致多次触发滚轮事件*/
	void OnWheelDelta(int x, int y, int delta) noexcept;
	void TrimBuffer() noexcept;
private:
	static constexpr unsigned int bufferSize = 16u;
	/*鼠标位置*/
	int x;
	int y;
	/*按键状态*/
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	/*是否在窗口内*/
	bool isInWindow = false;
	/*存储鼠标滚轮的总旋转值*/
	int wheelDeltaCarry = 0;
	/*事件队列*/
	std::queue<Event> buffer;
};

