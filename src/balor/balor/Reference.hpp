#pragma once

#include <balor/NonCopyable.hpp>


namespace balor {

class Referenceable;



/// Reference ƒNƒ‰ƒX‚Ì?‚ÉˆË‘¶‚µ‚È‚¢ŽÀ‘••”•ªB
class ReferenceBase {
protected:
	friend Referenceable;

protected:
	ReferenceBase();
	ReferenceBase(const ReferenceBase& value);
	ReferenceBase(Referenceable* pointer);
	~ReferenceBase();

protected:
	void _attach(Referenceable* pointer);
	void _detach();

protected:
	Referenceable* _pointer;
	ReferenceBase* _next;
};



/**
 * ƒNƒ‰ƒX‚Ìˆê‘Î‘½‚ÌŽQÆ?ƒCƒ“?‚ð?‚·B
 *
 * Referenceable ƒNƒ‰ƒX‚Ì”h¶ƒNƒ‰ƒX‚Ì?ƒCƒ“?‚ð´ëÀÔ‚·‚é‚±‚Æ‚ª‚Å‚«A‚»‚Ì?ƒCƒ“?‚Ì‚æ‚¤‚É‚Ó‚é‚Ü‚¤B
 * ŽQÆæ‚ÌƒfƒXƒgƒ‰ƒN?‚ªŒÄ‚Î‚ê‚é‚Æ?ƒCƒ“?‚ÍŽ©“®“I‚Éƒkƒ‹‚É‚È‚éB
 * ‚Ü‚½ŽQÆæ‚ª??ƒuƒZ?ƒ“ƒeƒBƒNƒX‚É‚æ‚Á‚ÄˆÚ“®‚µ‚½ŽžA?ƒCƒ“?‚àˆÚ“®æ‚ÉXV‚³‚ê‚éB
 *
 * C++ ‚Å‚ÍƒNƒ‰ƒX“¯Žm‚ÌŽQÆ‚Í shared_ptr ‚ðŽg‚¤Ž–‚ª‘½‚¢‚ª‚»‚Ìˆ×‚É‚ÍƒNƒ‰ƒX‚ðƒq?ƒvƒƒ‚ƒŠ‚ÉŠ„‚è“–‚Ä‚·‚é•K—v‚ª‚ ‚éB
 * ‰E•Ó’lŽQÆ‚ÌƒT??ƒg‚É‚æ‚Á‚ÄƒRƒs?‹Ö?ƒNƒ‰ƒX‚ð–³—‚â‚èƒX??ƒg?ƒCƒ“?‚ÅŠÇ—‚·‚é•K—v«‚ª”–‚ê‚½‚Ì‚Å
 * ƒNƒ‰ƒX“¯Žm‚ÌŽQÆ‚É‚Â‚¢‚Ä‚àƒƒ‚ƒŠŠ„‚è“–‚Ä‚ð‚·‚é‚±‚Æ‚È‚­‰ðŒˆ‚·‚éŽd‘g‚Ý‚ðì‚Á‚½B‚½‚¾‚µŽÀ’l‚È‚Ì‚Åƒ‰ƒCƒtƒTƒCƒNƒ‹‚ÌŠÇ—‚Í‚ ‚­‚Ü‚ÅŽè“®B
 *
 * <h3>EƒTƒ“ƒvƒ‹ƒR?ƒh</h3>
 * <pre><code>
	Frame frame(L"Reference Sample");

	Reference<Button> r;
	assert(!r);
	{
		Button button(frame, 20, 10, 0, 0, L"??ƒ“");

		r = &button;
		assert(r);
		assert(r->text() == L"??ƒ“");

		button = Button(frame, 20, 10, 0, 0, L"??ƒ“2");
		assert(!r); // button ‚Íì‚è’¼‚³‚ê‚½‚Ì‚ÅŽ©“®“I‚ÉŽQÆ‚à nullptr ‚É‚È‚éB

		r = &button;
		assert(r);
		assert(r->text() == L"??ƒ“2");
	}
	assert(!r); // button ‚Í”jŠü‚³‚ê‚½‚Ì‚ÅŽ©“®“I‚ÉŽQÆ‚à nullptr ‚É‚È‚éB
 * </code></pre>
 */
template<typename T>
class Reference : public ReferenceBase {
public:
	/// ƒkƒ‹?ƒCƒ“?‚Æ‚µ‚Äì¬B
	Reference() {}
	Reference(const Reference& value) : ReferenceBase(value) {}
	/// ŽQÆæ‚Ì?ƒCƒ“?‚©‚çì¬B
	Reference(T* pointer) : ReferenceBase(pointer) {}
	~Reference() {}

	Reference& operator=(const Reference& value) {
		if (this != &value) {
			_detach();
			_attach(value._pointer);
		}
		return *this;
	}
	/// ŽQÆæ‚Ì?ƒCƒ“?‚ð•ÏXB
	Reference& operator=(T* pointer) {
		_detach();
		_attach(pointer);
		return *this;
	}

public:
	/// ŽQÆæ‚Ì?ƒCƒ“?‚ÌŽæ“¾B
	operator T*() const { return static_cast<T*>(_pointer); } // ‚±‚±‚Å reinterpret_cast ‚ðŽg‚Á‚Ä‚Í‚È‚ç‚È‚¢B‰¼‘zŠÖ”‚ðŽ‚½‚È‚¢Šî’ê‚©‚çŽ‚Â”h¶ƒNƒ‰ƒX‚Ö‚ÌƒLƒƒƒXƒg‚ª³í‚É“®‚©‚È‚¢B
	/// ŽQÆæ‚Ö‚ÌƒAƒNƒZƒXB
	T* operator->() const { return static_cast<T*>(_pointer); }
};



/**
 * Reference ƒNƒ‰ƒX‚ÅŽQÆ‚Å‚«‚éƒNƒ‰ƒX‚ÌŠî’êƒNƒ‰ƒXB
 * ‚±‚ÌƒNƒ‰ƒX‚ðŒp³‚·‚éƒNƒ‰ƒX‚Å‚Í•K‚¸‰E•Ó’lŽQÆƒRƒ“ƒXƒgƒ‰ƒN?‚Æ‰E•Ó’lŽQÆ´ëÀÔ‰‰ŽZŽq‚ð’è?‚µ‚Ä
 * ‚±‚ÌƒNƒ‰ƒX‚Ì‰E•Ó’lŽQÆƒRƒ“ƒXƒgƒ‰ƒN?‚Æ‰E•Ó’lŽQÆ´ëÀÔ‰‰ŽZŽq‚ðŒÄ‚Ô•K—v‚ª‚ ‚éB
 * ŽÀÛ‚ÌŽg‚¢•û‚Í Reference ƒNƒ‰ƒX‚ÌƒhƒLƒ…ƒƒ“ƒg‚ðŽQÆB
 */
class Referenceable : private NonCopyable { // ˆê‘Î‘½ŽQÆ‚È‚Ì‚ÅƒRƒs?‹Ö?
public:
	friend ReferenceBase;

protected:
	Referenceable();
	/// ”h¶ƒNƒ‰ƒX‚©‚çŒÄ‚Ô•K—v‚ª‚ ‚éB
	Referenceable(Referenceable&& value);
	~Referenceable();
	/// ”h¶ƒNƒ‰ƒX‚©‚çŒÄ‚Ô•K—v‚ª‚ ‚éB
	Referenceable& operator=(Referenceable&& value);

private:
	ReferenceBase* _first;
};



}