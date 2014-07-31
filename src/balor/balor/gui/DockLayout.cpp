#include <algorithm>

#include "DockLayout.hpp"

#include <balor/gui/Scaler.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace gui {


using std::move;



bool DockLayout::Style::_validate(Style value) {
	return none <= value && value <= fill;
}


DockLayout::DockLayout() : _padding(0) {
}


DockLayout::DockLayout(DockLayout&& value)
	: _target(move(value._target))
	, _padding(move(value._padding))
	, _childInfos(move(value._childInfos))
	{
}


DockLayout::DockLayout(Control& target)
	: _target(&target)
	, _padding(0)
	{
}


DockLayout::~DockLayout() {
}


DockLayout& DockLayout::operator=(DockLayout&& value) {
	if (this != &value) {
		_target = move(value._target);
		_padding = move(value._padding);
		_childInfos = move(value._childInfos);
	}
	return *this;
}


Size DockLayout::getMinimumSize(Control& child) const {
	assert("Null target" && _target);
	assert("target not created" && *_target);
	assert("Invalid child" && child.parent() == _target);

	auto info = _findInfo(child);
	if (info) {
		return info->minimumSize;
	}
	return Size(0, 0);
}


DockLayout::Style DockLayout::getStyle(Control& child) const {
	assert("Null target" && _target);
	assert("target not created" && *_target);
	assert("Invalid child" && child.parent() == _target);

	auto info = _findInfo(child);
	if (info) {
		return info->style;
	}
	return Style::none;
}


Padding DockLayout::padding() const {
	return _padding;
}


void DockLayout::padding(const Padding& value) {
	_padding = value;
}


void DockLayout::padding(int left, int top, int right, int bottom) {
	_padding = Padding(left, top, right, bottom);
}


void DockLayout::perform() {
	assert("Null target" && _target);
	assert("target not created" && *_target);

	Rectangle layoutArea = _target->displayRectangle();
	layoutArea.x      += _padding.left;
	layoutArea.y      += _padding.top;
	layoutArea.width  = std::max(0, layoutArea.width  - _padding.horizontal());
	layoutArea.height = std::max(0, layoutArea.height - _padding.vertical());

	ChildInfo defaultInfo;
	for (auto i = _target->controlsBegin(); i; ++i) {
		if (!i->visibleExceptParent()) {
			continue;
		}

		auto info = _findInfo(*i);
		if (!info) {
			info = &defaultInfo;
		}
		if (info->style != Style::none) {
			switch (info->style) {
				case Style::top : {
					Size size(layoutArea.width, i->size().height);
					size = Size::maximize(size, info->minimumSize);
					i->bounds(layoutArea.x, layoutArea.y, size.width, size.height);
					layoutArea.y      += size.height;
					layoutArea.height -= size.height;
				} break;
				case Style::bottom : {
					Size size(layoutArea.width, i->size().height);
					size = Size::maximize(size, info->minimumSize);
					i->bounds(layoutArea.x, layoutArea.bottom() - size.height, size.width, size.height);
					layoutArea.height -= size.height;
				} break;
				case Style::left : {
					Size size(i->size().width, layoutArea.height);
					size = Size::maximize(size, info->minimumSize);
					i->bounds(layoutArea.x, layoutArea.y, size.width, size.height);
					layoutArea.x      += size.width;
					layoutArea.width  -= size.width;
				} break;
				case Style::right : {
					Size size(i->size().width, layoutArea.height);
					size = Size::maximize(size, info->minimumSize);
					i->bounds(layoutArea.right() - size.width, layoutArea.y, size.width, size.height);
					layoutArea.width  -= size.width;
				} break;
				case Style::fill : {
					Size size(layoutArea.size());
					size = Size::maximize(size, info->minimumSize);
					i->bounds(layoutArea.x, layoutArea.y, size.width, size.height);
				} break;
			}
		}
	}
}


Size DockLayout::preferredSize() const {
	assert("Null target" && _target);
	assert("target not created" && *_target);

	Padding parentMargin(0); // ドッキングコントロールがそれぞれの枠に占める大きさ
	Size minSize = Size(0, 0); // ドッキングコントロールの可変サイズ部分の最小値の合計
	ChildInfo defaultInfo;
	for (auto i = _target->controlsBegin(); i; ++i) {
		if (!i->visibleExceptParent()) {
			continue;
		}
		auto info = _findInfo(*i);
		if (!info) {
			info = &defaultInfo;
		}
		if (info->style != Style::none) {
			switch (info->style) {
				case Style::top :
				case Style::bottom : {
					int height = std::max(info->minimumSize.height, i->size().height);
					if (info->style == Style::top) {
						parentMargin.top    += height;
					} else {
						parentMargin.bottom += height;
					}
					minSize.width = std::max(minSize.width, parentMargin.horizontal() + info->minimumSize.width);
				} break;
				case Style::left :
				case Style::right : {
					int width = std::max(info->minimumSize.width, i->size().width);
					if (info->style == Style::left) {
						parentMargin.left   += width;
					} else {
						parentMargin.right  += width;
					}
					minSize.height = std::max(minSize.height, parentMargin.vertical() + info->minimumSize.height);
				} break;
				case Style::fill : {
					minSize = Size::maximize(minSize, parentMargin.size() +  info->minimumSize);
				} break;
			}
		}
	}
	Size preferredSizeForOther = Size(0, 0);
	auto displayRect = _target->displayRectangle();
	for (auto i = _target->controlsBegin(); i; ++i) {
		if (!i->visibleExceptParent()) {
			continue;
		}
		auto info = _findInfo(*i);
		if (!info) {
			info = &defaultInfo;
		}
		if (info->style == Style::none) {
			Rectangle bounds= i->bounds();
			bounds.position(bounds.position() - displayRect.position()); // スクロールを打ち消す
			auto margin = Padding(0);
			preferredSizeForOther.width = std::max(preferredSizeForOther.width, bounds.right() + margin.right + parentMargin.right); // parentMargin を加えることで dockコントロールと重ならないようにする
			preferredSizeForOther.height = std::max(preferredSizeForOther.height, bounds.bottom() + margin.bottom + parentMargin.bottom); // parentMargin を加えることで dockコントロールと重ならないようにする
		}
	}

	Size preferredSizeForDocking = parentMargin.size();
	preferredSizeForOther.width  -= _padding.left;
	preferredSizeForOther.height -= _padding.top;
	auto preferredSize = Size::maximize(preferredSizeForDocking, preferredSizeForOther);
	preferredSize = Size::maximize(preferredSize, minSize);
	return _target->sizeFromClientSize(preferredSize + _padding.size());
}


void DockLayout::scale(const Scaler& scaler) {
	padding(scaler.scale(padding()));
	for (auto i = _childInfos.begin(), end = _childInfos.end(); i != end; ++i) {
		i->minimumSize = scaler.scale(i->minimumSize);
	}
}


void DockLayout::setMinimumSize(Control& child, const Size value) {
	assert("Null target" && _target);
	assert("target not created" && *_target);
	assert("Invalid child" && child.parent() == _target);

	auto& info = _getInfo(child);
	info.minimumSize = value;
}


void DockLayout::setMinimumSize(Control& child, int width, int height) {
	setMinimumSize(child, Size(width, height));
}


void DockLayout::setStyle(Control& child, DockLayout::Style value) {
	assert("Null target" && _target);
	assert("target not created" && *_target);
	assert("Invalid child" && child.parent() == _target);
	assert("Invalid DockLayout::Style" && Style::_validate(value));

	auto& info = _getInfo(child);
	info.style = value;
}


Control* DockLayout::target() {
	return _target;
}


DockLayout::ChildInfo::ChildInfo()
	: style(Style::none)
	, minimumSize(0, 0) {
}


const DockLayout::ChildInfo* DockLayout::_findInfo(Control& child) const {
	for (auto i = _childInfos.begin(), end = _childInfos.end(); i != end; ++i) {
		if (i->child == &child) {
			return &(*i);
		}
	}
	return nullptr;
}


DockLayout::ChildInfo& DockLayout::_getInfo(Control& child) {
	for (auto i = _childInfos.begin(), end = _childInfos.end(); i != end; ++i) {
		if (i->child == &child) {
			return (*i);
		}
	}
	_childInfos.push_back(ChildInfo());
	ChildInfo& info = _childInfos.back();
	info.child = &child;
	return info;
}



	}
}