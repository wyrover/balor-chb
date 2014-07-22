#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {


/**
 * Button, CheckBox, RadioButton �̋��ʏ������܂Ƃ߂����N���X�B
 */
class ButtonBase : public Control {
public:
	/// ������̔z�u�B
	struct TextAlign {
		enum _enum {
			topLeft      = 0x00000500L, /// ����B
			topCenter    = 0x00000700L, /// �㒆���B
			topRight     = 0x00000600L, /// �E��B
			middleLeft   = 0x00000D00L, /// �������B
			middleCenter = 0x00000F00L, /// �����B
			middleRight  = 0x00000E00L, /// �����E�B
			bottomLeft   = 0x00000900L, /// �����B
			bottomCenter = 0x00000B00L, /// �������B
			bottomRight  = 0x00000A00L, /// �E���B
		};
		BALOR_NAMED_ENUM_MEMBERS(TextAlign);
	};


protected:
	ButtonBase();
	ButtonBase(ButtonBase&& value);
	virtual ~ButtonBase() = 0;

public:
	/// �����񂪂͂ݏo���ꍇ����s�R�[�h�ɂ���ĉ��s���邩�ǂ����B�����l�� false�B
	bool multiline() const;
	void multiline(bool value);
	/// �{�^������������Ԃ��ǂ����B
	bool pushed() const;
	/// ������̔z�u�B�����l�� Button �� ButtonBase::TextAlign::middleCenter�ACheckBox �� RadioButton �� middleLeft�B
	ButtonBase::TextAlign textAlign() const;
	void textAlign(ButtonBase::TextAlign value);
};



/**
 * �{�^���R���g���[���B
 *
 * �`�F�b�N�{�b�N�X�A���W�I�{�^���A�O���[�v�{�b�N�X�͂��ꂼ�� CheckBox, RadioButton, GroupBox �N���X�Ŏ�������Ă���B
 * BS_BITMAP �� BS_ICON �X�^�C���� XP �Ńr�W���A���X�^�C���������I�ɖ����ɂȂ�̂ŃT�|�[�g���Ȃ��B
 * �J�X�^���h���[�� BCM_SETIMAGELIST �͋����I�Ƀt�F�[�h�A�j���[�V������������̂ŃT�|�[�g���Ȃ��BonPaint �C�x���g�őΉ�����B
 *
 * <h3>�E�T���v���R�[�h</h3>
 * <pre><code>
	Frame frame(L"Button Sample");

	Button button(frame, 20, 10, 0, 0, L"button", [&] (Button::Click& ) {
		MsgBox::show(L"clicked");
	});

	frame.runMessageLoop();
 * </code></pre>
 */
class Button : public ButtonBase {
public:
	/// Button �̃C�x���g�̐e�N���X�B
	typedef EventWithSubclassSender<Button, Control::Event> Event;

	typedef Event Click;
	typedef PaintEvent<Button, Event> Paint;


public:
	/// �k���n���h���ō쐬�B
	Button();
	Button(Button&& value, bool checkSlicing = true);
	/// �e�A�ʒu�A�傫���A�����񂩂�쐬�B�傫�����O�ɂ���� getPreferredSize �֐��ŋ��߂�B
	Button(Control& parent, int x, int y, int width, int height, StringRange text, Listener<Button::Event&> onClick = Listener<Button::Event&>());
	virtual ~Button();
	Button& operator=(Button&& value);

public:
	/// Enter �L�[�����������Ƀt�H�[�J�X���Ȃ��Ƃ����������ɂȂ�{�^���̊O�����ǂ����B�����l�� false�B
	bool defaultButton() const;
	void defaultButton(bool value);
	/// ���݂̕������t�H���g������œK�ȑ傫�������߂�Bmultiline() �� true �̏ꍇ width ���Œ肷��ƍœK�� height �����܂�B
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// �{�^�����N���b�N�����C�x���g�B
	Listener<Button::Click&>& onClick();
	/// �`�悷��C�x���g�B
	Listener<Button::Paint&>& onPaint();
	/// �{�^�����������C�x���g�𔭐�������B
	void performClick();

protected:
	/// ���b�Z�[�W����������B������E�C���h�E�v���V�[�W���B
	virtual void processMessage(Message& msg);

private:
	Listener<Button::Click&> _onClick;
	Listener<Button::Paint&> _onPaint;
};



	}
}