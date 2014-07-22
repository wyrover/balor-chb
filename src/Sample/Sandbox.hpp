	//// FontDialog 긡긚긣
	//FontDialog dialog;
	////dialog.charsetUI(false);
	////dialog.color(Color::red());
	////dialog.effectsUI(false);
	////dialog.fixedPitchOnly(true);
	//dialog.font(Font::defaultGui());
	////dialog.scriptsOnly(true);
	////dialog.verticalFonts(false);
	//dialog.onInit() = [&] (FontDialog::Init& e) {
	//	e.position(0, 0);
	//	e.clientSize(600, 500);
	//};
	//Button b0(frame, 0, 0, 0, 0, L"open Font", [&] (Button::Click& ) {
	//	if (dialog.show(frame)) {
	//		frame.invalidate();
	//	}
	//});
	//frame.onPaint() = [&] (Frame::Paint& e) {
	//	auto g = e.graphics();
	//	g.brush(Color::control());
	//	g.clear();
	//	if (dialog.font()) {
	//		g.font(dialog.font());
	//	}
	//	g.textColor(dialog.color());
	//	g.backTransparent(true);
	//	g.drawText(L"aBc굙괶굛궇궋궎궑궓듌럻", 10, 200);
	//};




	//// ColorDialog 긡긚긣
	//ColorDialog dialog;
	////dialog.createColorUI(false);
	//dialog.onInit() = [&] (ColorDialog::Init& e) {
	//	e.position(0, 0);
	//	//e.clientSize(600, 500);
	//};
	//Button b0(frame, 0, 0, 0, 0, L"open Color", [&] (Button::Click& ) {
	//	//dialog.color(Color(2, 100, 100));
	//	if (dialog.show(frame)) {
	//		frameBrush = Brush(dialog.color());
	//		frame.brush(frameBrush);
	//	}
	//});




	//// Splitter 긡긚긣
	//Button b0(frame, 0, 0, 100, 800, L"b0");
	//Button b1(frame, 110, 410, 890, 390, L"b1");
	//Splitter s2(frame, 110, 400, 890, 10);
	//Button b2(frame, 110, 0, 890, 400, L"b1");
	//Splitter c(frame, 100, 0, 10, 800);
	//auto zo0 = c.zOrder();
	//c.zOrder(1);
	//auto zo = c.zOrder();
	//c.margin(Padding(100));
	//s2.margin(Padding(2));

	//DockLayout dockLayout(frame);
	//dockLayout.setStyle(b0, DockLayout::Style::left);
	//dockLayout.setStyle(c, DockLayout::Style::left);
	//dockLayout.setStyle(b1, DockLayout::Style::bottom);
	//dockLayout.setStyle(s2, DockLayout::Style::bottom);
	//dockLayout.setStyle(b2, DockLayout::Style::fill);
	//frame.onResized() = [&] (Frame::Resized& ) {
	//	dockLayout.perform();
	//};




	//// Rebar 긡긚긣

	////typedef MenuBar::ItemInfo Item;
	////Item fileMenuItems[] = {
	////	Item(L"둎궘(&O)\tCtrl+O", Key::Modifier::ctrl | Key::o),
	////	Item(L"뺎뫔(&S)\tCtrl+S", Key::Modifier::ctrl | Key::s),
	////	Item(),
	////	Item(L"뢎뿹(&X)\tAlt+F4", Key::Modifier::alt | Key::f4)
	////};
	////auto onSubMenuClick = [&] (Menu::Click& e) {
	////	e.sender().checked(!e.sender().checked());
	////};
	////Item subMenuItems[] = {
	////	Item(L"?긃긞긏0", onSubMenuClick),
	////	Item(L"?긃긞긏1", onSubMenuClick),
	////	Item(L"?긃긞긏2", onSubMenuClick)
	////};
	////Item editMenuItems[] = {
	////	Item(L"긖긳긽긦깄?(&S)", subMenuItems),
	////	Item(L"긽긦깄?귩뷄?렑(&H)", [&] (Menu::Click& ) {
	////		frame.menuBar(nullptr);
	////	})
	////};
	////Item menuItems[] = {
	////	Item(L"긲?귽깑(&F)", fileMenuItems),
	////	Item(L"뺂뢜(&E)", editMenuItems)
	////};
	////MenuBar menuBar(menuItems);
	////frame.menuBar(&menuBar);

	//layout.startPosition(0, 0);

	//// Rebar::ItemInfo 봹쀱궻띿맟
	//typedef ToolBar::ButtonInfo Info;
	//Info infos[] = {
	//	Info(ImageList::StandardImage::cut, L"먛귟롦귟", [&] (ToolBar::Click& ) { MsgBox::show(L"먛귟롦궯궫"); }),
	//	Info(ImageList::StandardImage::copy, L"긓긯?", [&] (ToolBar::Click& ) { MsgBox::show(L"긓긯?궢궫"); }),
	//	Info(ImageList::StandardImage::paste, L"?귟븊궚", [&] (ToolBar::Click& ) { MsgBox::show(L"?귟븊궚궫"); }),
	//	Info(ImageList::StandardImage::properties, L"긵깓긬긡귻"),
	//	Info(ImageList::StandardImage::fileNew, L"륷딮띿맟"),
	//	Info(ImageList::StandardImage::fileOpen, L"긲?귽깑귩둎궘"),
	//};
	//ImageList imageList(ImageList::SystemResource::largeStandardImages);
	//ToolBar tool(frame, 0, 0, 0, 0, imageList, infos);
	//tool.size(tool.buttonsSize().width, tool.size().height);
	//tool.transparent(!tool.transparent());
	//tool.hideClippedButtons(true);

	//const wchar_t* comboItems[] = {
	//	L"hoge",
	//	L"moge",
	//	L"moga",
	//};
	//ComboBox combo(frame, 0, 0, 0, 0, comboItems);
	//combo.selectedIndex(0);

	//Edit edit(frame, 0, 0, 0, 0, 10);
	//edit.size(edit.size().width, tool.buttonsSize().height);

	//Rebar::ItemInfo itemInfos[] = {
	//	Rebar::ItemInfo(tool, 250),
	//	Rebar::ItemInfo(combo, 0, L"궞귪귍"),
	//	Rebar::ItemInfo(edit),
	//	Rebar::ItemInfo(),
	//};
	//Bitmap itemBitmap(50, 50);
	//{
	//	Graphics g(itemBitmap);
	//	g.brush(Brush::control());
	//	g.clear();
	//	g.brush(Color::white());
	//	g.drawEllipse(0, 0, 50, 50);
	//}
	//itemInfos[0].bitmap(itemBitmap);
	//itemInfos[0].controlEdge(false);
	////itemInfos[0].resizable(false);
	//itemInfos[1].bitmap(itemBitmap);
	//itemInfos[1].bitmapOriginFixed(true);
	//itemInfos[2].lineBreak(true);
	//itemInfos[2].controlResizable(true);
	//itemInfos[2].displayWidth(itemInfos[2].minControlSize().width);
	//itemInfos[2].minControlSize(0, itemInfos[2].minControlSize().height);
	//itemInfos[2].onChevronClick() = [&] (Rebar::ChevronClick& e) {
	//	framePopupMenu.show(frame, e.chevronBounds().bottomLeft());
	//};

	//Rebar c(frame, itemInfos);

	//// Rebar::ItemInfo 궻긡긚긣
	//struct Compare {
	//	static bool equal(const Rebar::ItemInfo& lhs, const Rebar::ItemInfo& rhs) {
	//		return lhs.bitmap() == rhs.bitmap()
	//			&& lhs.bitmapOriginFixed() == rhs.bitmapOriginFixed()
	//			&& lhs.lineBreak() == rhs.lineBreak()
	//			&& lhs.control() == rhs.control()
	//			&& lhs.controlEdge() == rhs.controlEdge()
	//			&& lhs.controlResizable() == rhs.controlResizable()
	//			&& lhs.displayWidth() == rhs.displayWidth()
	//			&& lhs.minControlSize() == rhs.minControlSize()
	//			&& ((bool)lhs.onChevronClick() == (bool)rhs.onChevronClick())
	//			&& lhs.resizable() == rhs.resizable()
	//			&& lhs.text() == rhs.text()
	//			//&& lhs.width() == rhs.width()
	//			;
	//	}
	//	static bool equal(const Rebar::ItemInfo& lhs, Rebar::Item&& rhs) {
	//		return lhs.bitmap() == rhs.bitmap()
	//			&& lhs.bitmapOriginFixed() == rhs.bitmapOriginFixed()
	//			&& lhs.lineBreak() == rhs.lineBreak()
	//			&& lhs.control() == rhs.control()
	//			&& lhs.controlEdge() == rhs.controlEdge()
	//			&& lhs.controlResizable() == rhs.controlResizable()
	//			&& lhs.displayWidth() == rhs.displayWidth()
	//			&& lhs.minControlSize() == rhs.minControlSize()
	//			&& ((bool)lhs.onChevronClick() == (bool)rhs.onChevronClick())
	//			&& lhs.resizable() == rhs.resizable()
	//			&& lhs.text() == rhs.text()
	//			//&& lhs.width() == rhs.width()
	//			;
	//	}
	//};
	//{
	//	for (auto i = 0; i < c.count(); ++i) {
	//		assert(Compare::equal(itemInfos[i], c[i]));
	//	}
	//	auto newItemInfos = c.itemInfos();
	//	assert((int)newItemInfos.size() == c.count());
	//	for (auto i = 0; i < c.count(); ++i) {
	//		assert(Compare::equal(itemInfos[i], newItemInfos[i]));
	//	}
	//}

	//c.onMouseDown() = [&] (Rebar::MouseDown& e) {
	//	if (e.mButton()) {
	//		Debug::writeLine(String() + c.getIndexAt(e.position()));
	//	}
	//};
	//c.onResize() = [&] (Rebar::Resize& ) {
	//	layout.perform();
	//};
	//frame.onResized() = [&] (Frame::Resized& e) {
	//	c.size(frame.clientSize().width, c.size().height);
	//	c.invalidate();
	//	layout.perform();
	//	//for (auto i = 0; i < tool.count(); ++i) {
	//	//	if (tool.bounds().right() < tool.position().x + tool[i].bounds().right()) {
	//	//		Debug::writeLine(String() + L"invisi:" + i);
	//	//		break;
	//	//	}
	//	//}
	//};

	//Edit itemIndex(frame, 0, 0, 0, 0, 2);
	//itemIndex.text(L"0");

	//Button bitmap(frame, 0, 0, 0, 0, L"bitmap", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.bitmap(item.bitmap() ? nullptr : (Bitmap::HBITMAP)itemBitmap);
	//});
	//Button bitmapOriginFixed(frame, 0, 0, 0, 0, L"bitmapOriginFixed", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.bitmapOriginFixed(!item.bitmapOriginFixed());
	//});
	//Button bounds(frame, 0, 0, 0, 0, L"bounds", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	MsgBox::show(String() + item.bounds());
	//});
	//Button lineBreak(frame, 0, 0, 0, 0, L"lineBreak", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.lineBreak(!item.lineBreak());
	//});
	//Button control(frame, 0, 0, 0, 0, L"control", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	bool ok = item.control() != nullptr;
	//	item.minControlSize(ok ? Size(0, 0) : Size(70, tool.size().height * 2));
	//	item.control(ok ? nullptr : &tool);
	//});
	//Button controlEdge(frame, 0, 0, 0, 0, L"controlEdge", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.controlEdge(!item.controlEdge());
	//});
	//Button controlResizable(frame, 0, 0, 0, 0, L"controlResizable", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.controlResizable(!item.controlResizable());
	//});
	//Button displayWidth(frame, 0, 0, 0, 0, L"displayWidth", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	MsgBox::show(String() + item.displayWidth());
	//});
	//Edit displayWidthEdit(frame, 0, 0, 0, 0, 4);
	//displayWidthEdit.text(L"0");
	//Button displayWidthSet(frame, 0, 0, 0, 0, L"displayWidthSet", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.displayWidth(to<int>(displayWidthEdit.text()));
	//});
	//Button maximize(frame, 0, 0, 0, 0, L"maximize", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.maximize();
	//});
	//Button minimize(frame, 0, 0, 0, 0, L"minimize", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.minimize();
	//});
	//Button resizable(frame, 0, 0, 0, 0, L"resizable", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.resizable(!item.resizable());
	//});
	//Button text(frame, 0, 0, 0, 0, L"text", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	MsgBox::show(item.text());
	//});
	//Edit textEdit(frame, 0, 0, 0, 0, 4);
	//textEdit.text(L"text긡긚긣");
	//Button textSet(frame, 0, 0, 0, 0, L"textSet", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.text(textEdit.text());
	//});
	//Button width(frame, 0, 0, 0, 0, L"width", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	MsgBox::show(String() + item.width());
	//});
	//Edit widthEdit(frame, 0, 0, 0, 0, 4);
	//widthEdit.text(L"0");
	//Button widthSet(frame, 0, 0, 0, 0, L"widthSet", [&] (Button::Click& ) {
	//	auto item = c[to<int>(itemIndex.text())];
	//	item.width(to<int>(widthEdit.text()));
	//});
	//Button add(frame, 0, 0, 0, 0, L"add", [&] (Button::Click& ) {
	//	Rebar::ItemInfo info;
	//	info.text(L"귌궛");
	//	c.add(info);
	//});
	//Button clear(frame, 0, 0, 0, 0, L"clear", [&] (Button::Click& ) {
	//	c.clear();
	//});
	//Button count(frame, 0, 0, 0, 0, L"count", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c.count());
	//});
	//Button insert(frame, 0, 0, 0, 0, L"insert", [&] (Button::Click& ) {
	//	Rebar::ItemInfo info;
	//	info.text(L"귌궛귌궛");
	//	c.insert(to<int>(itemIndex.text()), info);
	//});
	//Button doubleClickToggle(frame, 0, 0, 0, 0, L"doubleClickToggle", [&] (Button::Click& ) {
	//	c.doubleClickToggle(!c.doubleClickToggle());
	//});
	//Button itemBorderLines(frame, 0, 0, 0, 0, L"itemBorderLines", [&] (Button::Click& ) {
	//	c.itemBorderLines(!c.itemBorderLines());
	//});
	//Button itemHeightVariable(frame, 0, 0, 0, 0, L"itemHeightVariable", [&] (Button::Click& ) {
	//	c.itemHeightVariable(!c.itemHeightVariable());
	//});
	//Button itemOrderFixed(frame, 0, 0, 0, 0, L"itemOrderFixed", [&] (Button::Click& ) {
	//	c.itemOrderFixed(!c.itemOrderFixed());
	//});
	//Button rowCount(frame, 0, 0, 0, 0, L"rowCount", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c.rowCount());
	//});




	//// SelectDirectoryDialog 긡긚긣
	//SelectDirectoryDialog dialog;
	//frame.brush(nullptr);

	//Label filePathLabel(frame, 0, 0, 0, 0, L"directoryPath");
	//Brush b0(Color::white());
	//filePathLabel.brush(b0);
	//Edit directoryPath(frame, 0, 0, 0, 0, 100, 1);

	//CheckBox helpButton(frame, 0, 0, 0, 0, L"helpButton");
	//helpButton.checked(dialog.helpButton());

	//CheckBox placesBar(frame, 0, 0, 0, 0, L"placesBar");
	//placesBar.checked(dialog.placesBar());

	//Label titleLabel(frame, 0, 0, 0, 0, L"title");
	//Edit title(frame, 0, 0, 0, 0, 10, 1);

	//Panel panel;
	//Button button;
	//Button openFileDialog(frame, 0, 0, 0, 0, L"openFileDialog", [&] (Button::Click& e) {
	//	dialog.directoryPath(directoryPath.text());
	//	dialog.helpButton(helpButton.checked());
	//	dialog.placesBar(placesBar.checked());
	//	dialog.title(title.text());
	//	
	//	if (dialog.show(frame)) {
	//		directoryPath.text(dialog.directoryPath());
	//		layout.perform();
	//	}
	//	//panel = Panel();
	//});

	//dialog.onInit() = [&] (SelectDirectoryDialog::Init& e) {
	//	e.position(0, 0);
	//	//e.hideControl(SelectDirectoryDialog::ControlId::filePathLabel);
	//	e.setControlText(SelectDirectoryDialog::ControlId::filePathLabel, L"긢귻깒긏긣깏");
	//	e.setControlText(SelectDirectoryDialog::ControlId::acceptButton, L"랷뤖");

	//	auto size = e.clientSize();
	//	panel = Panel::fromParentHandle(e.sender(), 0, size.height, 300, 100);
	//	panel.edge(Control::Edge::line);
	//	button = Button(panel, 0, 60, 0, 40, L"button", [&] (Button::Click& ) {
	//		MsgBox::show(L"button");
	//	});
	//	e.clientSize(size.width, size.height + panel.size().height);
	//};

	//dialog.onAccept() = [&] (SelectDirectoryDialog::Accept& e) {
	//	Debug::writeLine(L"path:" + e.filePath());
	//	Debug::writeLine(L"dir:" + e.directory());
	//	e.cancel(MsgBox::show(e.sender(), L"궓굥갎", L"궓굥", MsgBox::Buttons::okCancel) != MsgBox::Result::ok);
	//};

	//dialog.onHelpRequest() = [&] (SelectDirectoryDialog::HelpRequest& e) {
	//	MsgBox::show(e.sender(), L"help");
	//};




	//// SaveFileDialog 긡긚긣
	//SaveFileDialog dialog;
	//frame.brush(nullptr);

	//Label filePathLabel(frame, 0, 0, 0, 0, L"filePath");
	//Brush b0(Color::white());
	//filePathLabel.brush(b0);
	//Edit filePath(frame, 0, 0, 0, 0, 100, 1);

	//Label defaultExtentionLabel(frame, 0, 0, 0, 0, L"defaultExt");
	//Edit defaultExtention(frame, 0, 0, 0, 0, 10, 1);

	//ListBox filePaths(frame, 0, 0, 0, 0, dialog.filePaths());

	//Edit filterIndex(frame, 0, 0, 0, 0, 2);
	//filterIndex.text(String() + dialog.filterIndex());

	//CheckBox helpButton(frame, 0, 0, 0, 0, L"helpButton");
	//helpButton.checked(dialog.helpButton());

	//Label initialDirectoryLabel(frame, 0, 0, 0, 0, L"initialDirectory");
	//Edit initialDirectory(frame, 0, 0, 0, 0, 100, 1);

	//CheckBox multiselect(frame, 0, 0, 0, 0, L"multiselect");
	//multiselect.checked(dialog.multiselect());

	//CheckBox noDereferenceLinks(frame, 0, 0, 0, 0, L"noDereferenceLinks");
	//noDereferenceLinks.checked(dialog.noDereferenceLinks());

	//CheckBox overwritePrompt(frame, 0, 0, 0, 0, L"overwritePrompt");
	//overwritePrompt.checked(dialog.overwritePrompt());

	//CheckBox placesBar(frame, 0, 0, 0, 0, L"placesBar");
	//placesBar.checked(dialog.placesBar());

	//Label titleLabel(frame, 0, 0, 0, 0, L"title");
	//Edit title(frame, 0, 0, 0, 0, 10, 1);

	//Button saveFileDialog(frame, 0, 0, 0, 0, L"saveFileDialog", [&] (Button::Click& e) {
	//	dialog.filePath(filePath.text());
	//	dialog.defaultExtention(defaultExtention.text());
	//	dialog.filterIndex(to<int>(filterIndex.text()));
	//	dialog.helpButton(helpButton.checked());
	//	dialog.initialDirectory(initialDirectory.text());
	//	dialog.multiselect(multiselect.checked());
	//	dialog.overwritePrompt(overwritePrompt.checked());
	//	dialog.noDereferenceLinks(noDereferenceLinks.checked());
	//	dialog.placesBar(placesBar.checked());
	//	dialog.title(title.text());

	//	dialog.filter(L"멣궲궻긲?귽깑\n*.*\ndll긲?귽깑\n*.dll\n깋귽긳깋깏긲?귽깑\n*.lib\n\n");
	//	
	//	if (dialog.show(frame)) {
	//		filePath.text(dialog.filePath());
	//		filePaths.items(dialog.filePaths());
	//		filePaths.resize();
	//		filterIndex.text(String() + dialog.filterIndex());
	//		layout.perform();
	//	}
	//});

	//Panel panel;
	//Button button;
	//dialog.onInit() = [&] (SaveFileDialog::Init& e) {
	//	e.position(0, 0);
	//	e.hideControl(SaveFileDialog::ControlId::filePathLabel);
	//	e.setControlText(SaveFileDialog::ControlId::filterLabel, L"긲귻깑??궬귝");
	//	e.setControlText(SaveFileDialog::ControlId::cancelButton, L"귘귕귡갎궭귛궯궴뮮궋빒럻쀱귩볺귢궲귒귡갃");

	//	auto size = e.size();
	//	e.size(size.width, size.height + 200);
	//	panel = Panel::fromParentHandle(e.sender(), 0, size.height - 36, 300, 200);
	//	panel.edge(Control::Edge::line);
	//	button = Button(panel, 0, 60, 0, 40, L"button", [&] (Button::Click& ) {
	//		MsgBox::show(L"button");
	//	});
	//};

	////dialog.onAccept() = [&] (SaveFileDialog::Accept& e) {
	////	Debug::writeLine(L"path:" + e.filePath());
	////	Debug::writeLine(L"dir:" + e.directory());
	////	e.cancel(MsgBox::show(e.sender(), L"궓굥갎", L"궓굥", MsgBox::Buttons::okCancel) != MsgBox::Result::ok);
	////};

	//dialog.onHelpRequest() = [&] (SaveFileDialog::HelpRequest& e) {
	//	MsgBox::show(e.sender(), L"help");
	//};

	//dialog.onDirectoryChange() = [&] (SaveFileDialog::DirectoryChange& e) {
	//	Debug::writeLine(String() + L"dirChange:" + e.directory());
	//};

	//dialog.onSelect() = [&] (SaveFileDialog::Select& e) {
	//	Debug::writeLine(String() + L"select:" + e.filePath());
	//};




	//// OpenFileDialog 긡긚긣
	//OpenFileDialog dialog;
	//frame.brush(nullptr);

	//Label filePathLabel(frame, 0, 0, 0, 0, L"filePath");
	//Brush b0(Color::white());
	//filePathLabel.brush(b0);
	//Edit filePath(frame, 0, 0, 0, 0, 100, 1);

	//Label defaultExtentionLabel(frame, 0, 0, 0, 0, L"defaultExt");
	//Edit defaultExtention(frame, 0, 0, 0, 0, 10, 1);

	//CheckBox fileMustExist(frame, 0, 0, 0, 0, L"fileMustExist");
	//fileMustExist.checked(dialog.fileMustExist());

	//ListBox filePaths(frame, 0, 0, 0, 0, dialog.filePaths());

	//Edit filterIndex(frame, 0, 0, 0, 0, 2);
	//filterIndex.text(String() + dialog.filterIndex());

	//CheckBox helpButton(frame, 0, 0, 0, 0, L"helpButton");
	//helpButton.checked(dialog.helpButton());

	//Label initialDirectoryLabel(frame, 0, 0, 0, 0, L"initialDirectory");
	//Edit initialDirectory(frame, 0, 0, 0, 0, 100, 1);

	//CheckBox multiselect(frame, 0, 0, 0, 0, L"multiselect");
	//multiselect.checked(dialog.multiselect());

	//CheckBox noDereferenceLinks(frame, 0, 0, 0, 0, L"noDereferenceLinks");
	//noDereferenceLinks.checked(dialog.noDereferenceLinks());

	//CheckBox pathMustExist(frame, 0, 0, 0, 0, L"pathMustExist");
	//pathMustExist.checked(dialog.pathMustExist());

	//CheckBox placesBar(frame, 0, 0, 0, 0, L"placesBar");
	//placesBar.checked(dialog.placesBar());

	//CheckBox readOnly(frame, 0, 0, 0, 0, L"readOnly");
	//readOnly.checked(dialog.readOnly());

	//CheckBox readOnlyCheckBox(frame, 0, 0, 0, 0, L"readOnlyCheckBox");
	//readOnlyCheckBox.checked(dialog.readOnlyCheckBox());

	//Label titleLabel(frame, 0, 0, 0, 0, L"title");
	//Edit title(frame, 0, 0, 0, 0, 10, 1);

	//Button openFileDialog(frame, 0, 0, 0, 0, L"openFileDialog", [&] (Button::Click& e) {
	//	dialog.filePath(filePath.text());
	//	dialog.defaultExtention(defaultExtention.text());
	//	dialog.fileMustExist(fileMustExist.checked());
	//	dialog.filterIndex(to<int>(filterIndex.text()));
	//	dialog.helpButton(helpButton.checked());
	//	dialog.initialDirectory(initialDirectory.text());
	//	dialog.multiselect(multiselect.checked());
	//	dialog.noDereferenceLinks(noDereferenceLinks.checked());
	//	dialog.pathMustExist(pathMustExist.checked());
	//	dialog.placesBar(placesBar.checked());
	//	dialog.readOnly(readOnly.checked());
	//	dialog.readOnlyCheckBox(readOnlyCheckBox.checked());
	//	dialog.title(title.text());

	//	dialog.filter(L"멣궲궻긲?귽깑\n*.*\ndll긲?귽깑\n*.dll\n깋귽긳깋깏긲?귽깑\n*.lib\n\n");
	//	
	//	if (dialog.show(frame)) {
	//		filePath.text(dialog.filePath());
	//		filePaths.items(dialog.filePaths());
	//		filePaths.resize();
	//		filterIndex.text(String() + dialog.filterIndex());
	//		readOnly.checked(dialog.readOnly());
	//		layout.perform();
	//	}
	//});

	//Panel panel;
	//Button button;
	//dialog.onInit() = [&] (OpenFileDialog::Init& e) {
	//	e.position(0, 0);
	//	e.hideControl(OpenFileDialog::ControlId::filePathLabel);
	//	e.setControlText(OpenFileDialog::ControlId::filterLabel, L"긲귻깑??궬귝");
	//	e.setControlText(OpenFileDialog::ControlId::cancelButton, L"귘귕귡갎궭귛궯궴뮮궋빒럻쀱귩볺귢궲귒귡갃");

	//	auto size = e.size();
	//	e.size(size.width, size.height + 200);
	//	panel = Panel::fromParentHandle(e.sender(), 0, size.height - 36, 300, 200);
	//	panel.edge(Control::Edge::line);
	//	button = Button(panel, 0, 60, 0, 40, L"button", [&] (Button::Click& ) {
	//		MsgBox::show(L"button");
	//	});
	//};

	//dialog.onAccept() = [&] (OpenFileDialog::Accept& e) {
	//	Debug::writeLine(L"path:" + e.filePath());
	//	Debug::writeLine(L"dir:" + e.directory());
	//	e.cancel(MsgBox::show(e.sender(), L"궓굥갎", L"궓굥", MsgBox::Buttons::okCancel) != MsgBox::Result::ok);
	//};

	//dialog.onHelpRequest() = [&] (OpenFileDialog::HelpRequest& e) {
	//	MsgBox::show(e.sender(), L"help");
	//};

	//dialog.onDirectoryChange() = [&] (OpenFileDialog::DirectoryChange& e) {
	//	Debug::writeLine(String() + L"dirChange:" + e.directory());
	//};

	//dialog.onSelect() = [&] (OpenFileDialog::Select& e) {
	//	Debug::writeLine(String() + L"select:" + e.filePath());
	//};



	//// Tab 긡긚긣
	//Tab c(frame, 0, 0, 500, 200);

	//Panel page0(c, 0, 0, 0, 0);
	//Button b01(page0, 10, 10, 0, 0, L"b01");
	//Button b02(page0, 10, 60, 0, 0, L"b02");

	//Panel page1(c, 0, 0, 0, 0);
	//RadioButton r11(page1, 10, 10, 0, 0, L"r11");
	//RadioButton r12(page1, 10, 60, 0, 0, L"r12");

	//typedef Tab::ItemInfo Info;
	//Info infos[] = {Info(L"?긳0", page0, 0)
	//			   ,Info(L"?긳1귌궛", page1, 1)
	//			   ,Info(L"?긳2", 2)
	//};
	//c.itemInfos(infos);



	//// ToolBar 긡긚긣
	//layout.startPosition(0, 0);

	//// ??깛긢??
	//typedef ToolBar::ButtonInfo Info;
	//typedef ToolBar::ButtonStyle Style;
	//typedef ToolBar::Click Click;
	//struct Compare {
	//	static bool equals(const Info& lhs, const Info& rhs) {
	//		return lhs.checked() == rhs.checked()
	//			&& lhs.enabled() == rhs.enabled()
	//			//&& lhs.imageIndex() == rhs.imageIndex()
	//			&& (lhs.imageIndex() == rhs.imageIndex() || (lhs.style() == Style::separator
	//				&& (lhs.imageIndex() == 0 || lhs.imageIndex() == 8) && (rhs.imageIndex() == 0 || rhs.imageIndex() == 8)))
	//			&& lhs.noPrefix() == rhs.noPrefix()
	//			&& lhs.rightText() == rhs.rightText()
	//			&& lhs.style() == rhs.style()
	//			&& lhs.text() == rhs.text()
	//			&& lhs.visible() == rhs.visible()
	//			;
	//	}
	//};
	//PopupMenu dropDownMenu;
	//{
	//	typedef Menu::ItemInfo Item;
	//	Item items[] = {
	//		 Item(L"긤깓긞긵괥", [&] (Menu::Event& e) { e.sender().checked(!e.sender().checked()); })
	//		,Item(L"긤깓긞긵괦", [&] (Menu::Event& ) { MsgBox::show(L"긤깓긞긵괦"); })
	//		,Item(L"긤깓긞긵괧", [&] (Menu::Event& ) { MsgBox::show(L"긤깓긞긵괧"); })
	//	};
	//	dropDownMenu = PopupMenu(items);
	//}
	//Info infos[] = {Info(0, L"?뽞00??깛&A", [&] (Click& ) { MsgBox::show(L"?뽞00"); })
	//			   ,Info()
	//			   ,Info(1, L"?뽞02?긃긞긏&B", Style::checkBox, [&] (Click& ) { Debug::writeLine(L"?뽞02"); })
	//			   ,Info(2, L"?뽞03?긃긞긏", Style::checkBox, [&] (Click& ) { Debug::writeLine(L"?뽞03"); })
	//			   ,Info(3, L"?뽞04깋긙긆", Style::radioButton, [&] (Click& ) { Debug::writeLine(L"?뽞04"); })
	//			   ,Info(4, L"?뽞05깋긙긆", Style::radioButton, [&] (Click& ) { Debug::writeLine(L"?뽞05"); })
	//			   ,Info(5, L"?뽞06깋긙긆", Style::radioButton, [&] (Click& ) { Debug::writeLine(L"?뽞06"); })
	//			   ,Info(6, L"?뽞07긤깓긞긵", Style::dropDown, [&] (Click& e) {
	//					if (e.dropDown()) {
	//						dropDownMenu.show(e.sender(), e.button().bounds().bottomLeft());
	//					}
	//					Debug::writeLine(L"?뽞07");
	//				})
	//			   ,Info(7, L"?뽞08긤깓긞긵??깛", Style::dropDownButton, [&] (Click& e) {
	//					if (e.dropDown()) {
	//						dropDownMenu.show(e.sender(), e.button().bounds().bottomLeft());
	//					} else {
	//						MsgBox::show(L"?뽞괥괰");
	//					}
	//				})
	//			   ,Info(8, L"?뽞&09", [&] (Click& ) { MsgBox::show(L"?뽞09"); })
	//			   ,Info(9, L"?뽞10", [&] (Click& ) { MsgBox::show(L"?뽞10"); })
	//			   ,Info(10, L"?뽞&11", [&] (Click& ) { MsgBox::show(L"?뽞11"); })
	//			   ,Info()
	//			   ,Info(11, L"?뽞13", [&] (Click& ) { MsgBox::show(L"?뽞13"); })
	//			   };

	//infos[2].checked(true);
	//infos[2].onButtonTipPopup() = [&] (ToolBar::ButtonTipPopup& e) {
	//	e.setText(L"?뽞괥괧?긞긵");
	//};
	//infos[5].checked(true);
	////infos[8].enabled(false);
	//infos[9].rightText(true);
	//infos[10].style(Style::radioButton);
	//infos[10].noPrefix(true);
	//infos[10].text(L"?뽞&10");
	//infos[11].visible(false);
	//infos[12].separatorSize(100);

	//// 귽긽?긙깏긚긣
	//ImageList list = ImageList::fromSystemResource(ImageList::SystemResource::largeStandardImages);
	////ImageList list(100, 100);
	////Bitmap bm0(400, 100);
	////{
	////	Graphics g(bm0);
	////	g.brush(Color::black());
	////	g.clear();
	////	g.brush(Color::red());
	////	g.pen(Color::red());
	////	g.drawEllipse(0, 0, 100, 100);
	////	g.brush(Color::blue());
	////	g.pen(Color::blue());
	////	g.drawEllipse(100, 0, 100, 100);
	////	g.brush(Color::green());
	////	g.pen(Color::green());
	////	g.drawEllipse(200, 0, 100, 100);
	////	g.brush(Color::white());
	////	g.pen(Color::white());
	////	g.drawEllipse(300, 0, 100, 100);
	////}
	////list.add(bm0, Color::black());

	//ToolBar c(frame, 0, 0, 0, 0, list, infos);

	//// 긟?깑긫?뤵궸긓깛긣깓?깑귩뭫궘
	//Edit editOnToolBar(c, 0, 0, 0, 0);
	//editOnToolBar.bounds(c[12].bounds());

	//c.onMouseDown() = [&] (Control::MouseDown& e) {
	//	if (e.lButton()) {
	//		auto index = c.getIndexAt(e.position());
	//		if (0 <= index) { // pushed() 궻긡긚긣
	//			for (int i = 0; i < c.count(); ++i) {
	//				if (c[i].style() != ToolBar::ButtonStyle::dropDown && c[i].style() != ToolBar::ButtonStyle::dropDownButton) {
	//					assert(c[i].pushed() == (c[i].enabled() && i == index));
	//				}
	//			}
	//		}
	//	} else if (e.mButton()) { // getIndexAt 궻긡긚긣
	//		Debug::writeLine(String() + L"button index = " + c.getIndexAt(e.position()));
	//	}
	//};

	//// buttonInfos갂??깛봹쀱륃뺪궻롦벦궴뵿뎕궻긡긚긣
	//{
	//	auto infos2 = c.buttonInfos();
	//	assert(infos2.size() == sizeof(infos) / sizeof(infos[0]));
	//	for (int i = 0; i < (int)infos2.size(); ++i) {
	//		assert(Compare::equals(infos[i], infos2[i]));
	//	}
	//}

	//Info infosB[] = {Info(0, L"괧?뽞00??깛", Style::checkBox, [&] (Click& ) { MsgBox::show(L"괧?뽞00"); })
	//			   ,Info(1, L"괧?뽞02?긃긞긏", [&] (Click& ) { Debug::writeLine(L"괧?뽞02"); })
	//			   ,Info()
	//			   };
	//c.buttonInfos(infosB);
	//{
	//	auto infos2 = c.buttonInfos();
	//	assert(infos2.size() == sizeof(infosB) / sizeof(infosB[0]));
	//	for (int i = 0; i < (int)infos2.size(); ++i) {
	//		assert(Compare::equals(infosB[i], infos2[i]));
	//	}
	//}
	//c.buttonInfos(infos);
	//{
	//	auto infos2 = c.buttonInfos();
	//	assert(infos2.size() == sizeof(infos) / sizeof(infos[0]));
	//	for (int i = 0; i < (int)infos2.size(); ++i) {
	//		assert(Compare::equals(infos[i], infos2[i]));
	//	}
	//}

	//// ??깛긡긚긣UI
	//Edit buttonIndexEdit(frame, 0, 0, 0, 0, 2);
	//buttonIndexEdit.text(L"0");

	//Button autoSize(frame, 0, 0, 0, 0, L"autoSize", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].autoSize());
	//});
	//Button autoSizeSet(frame, 0, 0, 0, 0, L"autoSizeSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.autoSize(!button.autoSize());
	//});
	//Button buttonBounds(frame, 0, 0, 0, 0, L"buttonBounds", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].bounds());
	//});
	//Button buttonChecked(frame, 0, 0, 0, 0, L"buttonChecked", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].checked());
	//});
	//Button buttonCheckedSet(frame, 0, 0, 0, 0, L"buttonCheckedSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.checked(!button.checked());
	//});
	//Button buttonEnabled(frame, 0, 0, 0, 0, L"buttonEnabled", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].enabled());
	//});
	//Button buttonEnabledSet(frame, 0, 0, 0, 0, L"buttonEnabledSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.enabled(!button.enabled());
	//});
	//Button buttonImageIndex(frame, 0, 0, 0, 0, L"buttonImageIndex", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].imageIndex());
	//});
	//Edit buttonImageIndexEdit(frame, 0, 0, 0, 0, 2);
	//buttonImageIndexEdit.text(L"0");
	//Button buttonImageIndexSet(frame, 0, 0, 0, 0, L"buttonImageIndexSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.imageIndex(to<int>(buttonImageIndexEdit.text()));
	//});
	//Button buttonIndex(frame, 0, 0, 0, 0, L"buttonIndex", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].index());
	//});
	//Button buttonNoPrefix(frame, 0, 0, 0, 0, L"buttonNoPrefix", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].noPrefix());
	//});
	//Button buttonNoPrefixSet(frame, 0, 0, 0, 0, L"buttonNoPrefixSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.noPrefix(!button.noPrefix());
	//});
	//Button buttonRightText(frame, 0, 0, 0, 0, L"buttonRightText", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].rightText());
	//});
	//Button buttonRightTextSet(frame, 0, 0, 0, 0, L"buttonRightTextSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.rightText(!button.rightText());
	//});
	//const wchar_t* buttonStyleTexts[] = {L"button"
	//									,L"separator"
	//									,L"checkBox"
	//									,L"radioButton"
	//									,L"dropDown"
	//									,L"dropDownButton"
	//};
	//const ToolBar::ButtonStyle buttonStyles[] = {ToolBar::ButtonStyle::button
	//											,ToolBar::ButtonStyle::separator
	//											,ToolBar::ButtonStyle::checkBox
	//											,ToolBar::ButtonStyle::radioButton
	//											,ToolBar::ButtonStyle::dropDown
	//											,ToolBar::ButtonStyle::dropDownButton
	//};
	//ComboBox buttonStyleCombo(frame, 0, 0, 0, 0, buttonStyleTexts, ComboBox::Style::dropDownList);
	//buttonStyleCombo.selectedIndex(0);
	//Button buttonStyle(frame, 0, 0, 0, 0, L"buttonStyle", [&] (Button::Click& ) {
	//	auto style = c[to<int>(buttonIndexEdit.text())].style();
	//	for (int i = 0; i < sizeof(buttonStyles) / sizeof(buttonStyles[0]); ++i) {
	//		if (style == buttonStyles[i]) {
	//			MsgBox::show(String() + buttonStyleTexts[i]);
	//		}
	//	}
	//});
	//Button buttonStyleSet(frame, 0, 0, 0, 0, L"buttonStyleSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.style(buttonStyles[buttonStyleCombo.selectedIndex()]);
	//});
	//Edit buttonTextEdit(frame, 0, 0, 0, 0, 10);
	//buttonTextEdit.text(L"text긡긚긣");
	//Button buttonText(frame, 0, 0, 0, 0, L"buttonText", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].text());
	//});
	//Button buttonTextSet(frame, 0, 0, 0, 0, L"buttonTextSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.text(buttonTextEdit.text());
	//});
	//Button buttonVisible(frame, 0, 0, 0, 0, L"buttonVisible", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].visible());
	//});
	//Button buttonVisibleSet(frame, 0, 0, 0, 0, L"buttonVisibleSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.visible(!button.visible());
	//});
	//Button buttonWidth(frame, 0, 0, 0, 0, L"buttonWidth", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].width());
	//});
	//Edit buttonWidthEdit(frame, 0, 0, 0, 0, 2);
	//buttonWidthEdit.text(L"0");
	//Button buttonWidthSet(frame, 0, 0, 0, 0, L"buttonWidthSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.width(to<int>(buttonWidthEdit.text()));
	//});
	//Button buttonWrap(frame, 0, 0, 0, 0, L"buttonWrap", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c[to<int>(buttonIndexEdit.text())].wrap());
	//});
	//Button buttonWrapSet(frame, 0, 0, 0, 0, L"buttonWrapSet", [&] (Button::Click& ) {
	//	auto button =  c[to<int>(buttonIndexEdit.text())];
	//	button.wrap(!button.wrap());
	//});



	//// ToolBar ?뫬긡긚긣굏괿
	//Button add(frame, 0, 0, 0, 0, L"add", [&] (Button::Click& ) {
	//	c.add(Info(to<int>(buttonImageIndexEdit.text()), buttonTextEdit.text(), buttonStyles[buttonStyleCombo.selectedIndex()]
	//	, [&] (Click& ) {
	//		//MsgBox::show(buttonTextEdit.text());
	//	}));
	//});
	//Button bottomText(frame, 0, 0, 0, 0, L"bottomText", [&] (Button::Click& ) {
	//	c.bottomText(!c.bottomText());
	//	c.resize();
	//	layout.perform();
	//});
	////Button buttonSize(frame, 0, 0, 0, 0, L"buttonSize", [&] (Button::Click& ) {
	////	MsgBox::show(String() + c.buttonSize().width + L", " + c.buttonSize().height);
	////});
	//auto buttonTip = c.buttonTip();
	//buttonTip.textColor(Color::red());
	//Button clear(frame, 0, 0, 0, 0, L"clear", [&] (Button::Click& ) {
	//	c.clear();
	//});
	//Button count(frame, 0, 0, 0, 0, L"count", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c.count());
	//});
	//assert(c.disabledImageList() == nullptr);
	//ImageList disabledImageList = ImageList::fromSystemResource(ImageList::SystemResource::largeViewImages);
	//Button disabledImageListChange(frame,0, 0, 0, 0, L"disabledImageListChange", [&] (Button::Event& ) {
	//	c.disabledImageList(disabledImageList);
	//});
	//Button divider(frame, 0, 0, 0, 0, L"divider", [&] (Button::Click& ) {
	//	c.divider(!c.divider());
	//});
	//Button edge(frame,0, 0, 0, 0, L"edge", [&] (Button::Event& ) {
	//	int edge = c.edge();
	//	++edge;
	//	if (3 < edge) {
	//		edge = 0;
	//	}
	//	c.edge(Control::Edge(edge));
	//	c.resize();
	//});
	//Button focusedIndex(frame, 0, 0, 0, 0, L"focusedIndex", [&] (Button::Click& ) {
	//	MsgBox::show(String() + c.focusedIndex());
	//});
	//Button focusedIndexSet(frame, 0, 0, 0, 0, L"focusedIndexSet", [&] (Button::Click& ) {
	//	c.focus();
	//	int index = to<int>(buttonIndexEdit.text());
	//	c.focusedIndex(index);
	//	assert(c.focusedIndex() == index);
	//});
	//Button hideClippedButtons(frame, 0, 0, 0, 0, L"hideClippedButtons", [&] (Button::Click& ) {
	//	c.hideClippedButtons(!c.hideClippedButtons());
	//});
	//assert(c.hotImageList() == nullptr);
	//Button hotImageListChange(frame,0, 0, 0, 0, L"hotImageListChange", [&] (Button::Event& ) {
	//	c.hotImageList(disabledImageList); // 럊궋귏귦궢
	//});
	//assert(c.imageList() == (ImageList::HIMAGELIST)list);
	//Button imageListUpdate(frame,0, 0, 0, 0, L"imageListUpdate", [&] (Button::Event& ) {
	//	static int index = 0;
	//	Icon icon;
	//	switch (++index) {
	//		case 0 : icon = Icon::error(); break;
	//		case 1 : icon = Icon::application(); break;
	//		case 2 : icon = Icon::asterisk(); index = -1; break;
	//	}
	//	list.replace(0, icon);
	//	c.invalidate(); // 뷠?갏
	//});
	//Button imageListChangeSmall(frame,0, 0, 0, 0, L"imageListChangeSmall", [&] (Button::Event& ) {
	//	list = ImageList::fromSystemResource(ImageList::SystemResource::smallStandardImages);
	//	c.imageList(list); // 덇뱗 clear() 궢궲 add() 궢궲 resize() 궢궶궋궴묈궖궠궼빾귦귞궶궋갃
	//});
	//Button imageListChange(frame,0, 0, 0, 0, L"imageListChange", [&] (Button::Event& ) {
	//	list = ImageList::fromSystemResource(ImageList::SystemResource::largeViewImages);
	//	c.imageList(list);
	//	c.invalidate();
	//});
	//Button imageListNull(frame,0, 0, 0, 0, L"imageListNull", [&] (Button::Event& ) {
	//	c.imageList(nullptr);
	//});
	//Edit indentEdit(frame, 0, 0, 0, 0, 3);
	//indentEdit.text(L"0");
	//Button indent(frame, 0, 0, 0, 0, L"indent", [&] (Button::Click& ) {
	//	c.indent(to<int>(indentEdit.text()));
	//	//c.invalidate();
	//});
	//Edit insertIndex(frame, 0, 0, 0, 0, 2);
	//insertIndex.text(L"0");
	//Button insert(frame, 0, 0, 0, 0, L"insert", [&] (Button::Click& ) {
	//	c.insert(to<int>(insertIndex.text()), Info(to<int>(buttonImageIndexEdit.text()), buttonTextEdit.text(), buttonStyles[buttonStyleCombo.selectedIndex()]
	//	, [&] (Click& ) {
	//		//MsgBox::show(buttonTextEdit.text());
	//	}));
	//});
	//Button resize(frame, 0, 0, 0, 0, L"resize", [&] (Button::Click& ) {
	//	c.resize();
	//});
	//Button vertical(frame, 0, 0, 0, 0, L"vertical", [&] (Button::Click& ) {
	//	c.vertical(!c.vertical());
	//	c.resize();
	//});
	//Button wrappable(frame, 0, 0, 0, 0, L"wrappable", [&] (Button::Click& ) {
	//	c.wrappable(!c.wrappable());
	//	c.resize();
	//});

	//frame.onResized() = [&] (Frame::Resized& ) {
	//	if (c.vertical()) {
	//		//c.size(c.size().width, frame.clientSize().height);
	//	} else {
	//		c.resize();
	//		//c.size(frame.clientSize().width, c.size().height);
	//	}
	//	layout.perform();
	//};




	//// UpDown 긡긚긣
	//Button b0(frame, 0, 0, 0, 0, L"b0");
	//Edit edit(frame, 200, 200, 150, 30, 10, 1);
	////edit.margin(10, 10, 10, 10);
	////edit.resize();
	////edit.edge(Control::Edge::none);
	//auto editSize = edit.size();
	//UpDown cc(frame, edit);
	////UpDown cc(frame, 0, 0, 20, 30);
	//UpDown c(move(cc));
	//cc = move(c);
	//c = move(cc);

	////edit.position(400, 100);
	////edit.size(200, 30);

	//edit.text(L"0");
	//c.onUp() = [&] (UpDown::Event& ) {
	//	edit.text(String() + (to<int>(edit.text()) + 1));
	//};
	//c.onDown() = [&] (UpDown::Event& ) {
	//	edit.text(String() + (to<int>(edit.text()) - 1));
	//};
	//Button leftAlign(frame, 0, 0, 0, 0, L"leftAlign", [&] (Button::Event& ) {
	//	auto onUp = c.onUp();
	//	auto onDown = c.onDown();
	//	c = UpDown(frame, *c.buddy(), UpDown::Options(c.options() ^ UpDown::Options::leftAlign));
	//	c.zOrder(edit.zOrder() - 1);
	//	c.onUp() = onUp;
	//	c.onDown() = onDown;
	//});
	//Button vertical(frame, 0, 0, 0, 0, L"vertical", [&] (Button::Event& ) {
	//	auto onUp = c.onUp();
	//	auto onDown = c.onDown();
	//	c = UpDown(frame, *c.buddy(), UpDown::Options(c.options() ^ UpDown::Options::horizontal));
	//	c.zOrder(edit.zOrder() - 1);
	//	c.onUp() = onUp;
	//	c.onDown() = onDown;
	//});
	//Button arrowKeys(frame, 0, 0, 0, 0, L"arrowKeys", [&] (Button::Event& ) {
	//	auto onUp = c.onUp();
	//	auto onDown = c.onDown();
	//	c = UpDown(frame, *c.buddy(), UpDown::Options(c.options() ^ UpDown::Options::noArrowKeys));
	//	c.zOrder(edit.zOrder() - 1);
	//	c.onUp() = onUp;
	//	c.onDown() = onDown;
	//});
	//Button buddy(frame, 0, 0, 0, 0, L"buddy", [&] (Button::Event& ) {
	//	c.buddy(&b0);
	//});

	//frame.onResized() = [&] (Frame::Resized& ) {
	//	c.updatable(false);
	//	layout.perform();
	//	c.buddy(&edit);
	//	//c.size(40, c.size().height);
	//	c.updatable(true);
	//	c.invalidate();
	//	c.update();
	//};




	///// IME 듫쁀긡긚긣
	//Edit edit0(frame, 0, 0, 0, 0);
	//Edit edit1(frame, 0, 0, 0, 0);

	//Button on(frame, 0, 0, 0, 0, L"on", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::on);
	//});
	//Button off(frame, 0, 0, 0, 0, L"off", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::off);
	//});
	//Button disabled(frame, 0, 0, 0, 0, L"disabled", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::disabled);
	//});
	//Button hiragana(frame, 0, 0, 0, 0, L"hiragana", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::hiragana);
	//});
	//Button katakana(frame, 0, 0, 0, 0, L"katakana", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::katakana);
	//});
	//Button katakanaHalf(frame, 0, 0, 0, 0, L"katakanaHalf", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::katakanaHalf);
	//});
	//Button alpha(frame, 0, 0, 0, 0, L"alpha", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::alpha);
	//});
	//Button alphaHalf(frame, 0, 0, 0, 0, L"alphaHalf", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeMode(Edit::ImeMode::alphaHalf);
	//});
	//Button general(frame, 0, 0, 0, 0, L"general", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeConvert(Edit::ImeConvert::general);
	//});
	//Button personAndPlaceName(frame, 0, 0, 0, 0, L"personAndPlaceName", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeConvert(Edit::ImeConvert::personAndPlaceName);
	//});
	//Button informal(frame, 0, 0, 0, 0, L"informal", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeConvert(Edit::ImeConvert::informal);
	//});
	//Button none(frame, 0, 0, 0, 0, L"none", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeConvert(Edit::ImeConvert::none);
	//});
	//Button imeFixed(frame, 0, 0, 0, 0, L"imeFixed", [&] (Button::Event& ) {
	//	//edit0.focus();
	//	edit0.imeFixed(!edit0.imeFixed());
	//});




	//// TreeView 긡긚긣
	//typedef TreeView::ItemInfo Item;

	//Item items02[] = {Item(L"?뽞괧괥"), Item(L"?뽞괧괦"), Item(L"?뽞괧괧")};

	//Item items0[] = {Item(L"?뽞괥", 0, 0, 0)
	//				,Item(L"?뽞괦", 1, 1, 1)
	//				,Item(L"?뽞괧", items02, 2, 2, 2)
	//				};
	////items0[0].bold(true);
	////items0[0].state(0);
	////items0[1].cut(true);
	////items0[1].state(1);
	////items0[2].highlight(true);
	//TreeView c(frame, 0, 0, 300, 200, items0, TreeView::Options::checkBoxes);
	//ImageList imageList(Icon::smallSize());
	//imageList.add(Icon::application());
	//imageList.add(Icon::asterisk());
	//imageList.add(Icon::error());
	//Edit edit;
	//Font font2(c.font(), c.font().height(), Font::Style::bold);
	//ToolTip toolTip;
	//DragDrop::Source source;
	//DragDrop::Target target;
	//auto item = c.root().front();;
	//auto setter = [&] () {
	//	c.imageList(imageList);
	//	c.lines(false);
	//	c.fullRowSelect(true);
	//	c.textEditable(true);
	//	c.onTextEditing() = [&] (TreeView::TextEditing& e) {
	//		edit = e.edit();
	//		edit.textColor(Color::red());
	//	};
	//	c.onMouseDown() = [&] (Control::MouseDown& e) {
	//		auto item = c.getItemAt(e.position());
	//		Debug::writeLine(String() + (item ? item.text() : String(L"null")));
	//	};
	//	c.onClick() = [&] (TreeView::Click& ) {
	//		Debug::writeLine(L"onClick");
	//	};
	//	c.onRightClick() = [&] (TreeView::RightClick& ) {
	//		Debug::writeLine(L"onRightClick");
	//	};
	//	c.onItemPrePaint() = [&] (TreeView::ItemPrePaint& e) {
	//		if (e.selected()) {
	//			//e.selected(false);
	//			e.focused(false);
	//			e.backColor(Color::red());
	//			e.textColor(Color::yellow());
	//			e.font(font2);
	//		} else if (e.item().highlight()) {
	//			e.backColor(Color::blue());
	//			e.textColor(Color::yellow());
	//		} else {
	//			e.backColor(Color::control());
	//			e.textColor(Color::green());
	//		}
	//	};
	//	//c.backColor(Color::yellow());
	//	c.itemTipEnabled(true);
	//	c.onItemTipPopup() = [&] (TreeView::ItemTipPopup& e) {
	//		e.setText(e.item().text());
	//	};
	//	toolTip = c.itemTip();
	//	toolTip.textColor(Color::red());

	//	source = DragDrop::Source(c);
	//	target = DragDrop::Target(c);
	//	c.onItemDrag() = [&] (TreeView::ItemDrag& e) {
	//		if (!e.rButton()) {
	//			source.doDragDrop(e.item().text(), DragDrop::Effect::move, c.imageList(), e.item().imageIndex(), 8, 8);
	//		}
	//	};
	//	frame.onClosed() = [&] (Frame::Closed& ) {
	//		source = DragDrop::Source();
	//		target = DragDrop::Target();
	//	};
	//	item = c.root().front();;
	//};
	//setter();

	//Button bold(frame, 0, 0, 0, 0, L"bold", [&] (Button::Event& ) {
	//	item.bold(!item.bold());
	//});
	//Button checked(frame, 0, 0, 0, 0, L"checked", [&] (Button::Event& ) {
	//	item.checked(!item.checked());
	//});
	//Button cut(frame, 0, 0, 0, 0, L"cut", [&] (Button::Event& ) {
	//	item.cut(!item.cut());
	//});
	//Button highlight(frame, 0, 0, 0, 0, L"highlight", [&] (Button::Event& ) {
	//	item.highlight(!item.highlight());
	//});
	//Button checkBoxes(frame, 0, 0, 0, 0, L"checkBoxes", [&] (Button::Event& ) {
	//	source = DragDrop::Source();
	//	target = DragDrop::Target();
	//	auto b = c.bounds();
	//	c = TreeView(*c.parent(), b.x, b.y, b.width, b.height, c.root().itemInfos(), TreeView::Options(c.options() ^ TreeView::Options::checkBoxes));
	//	setter();
	//});




	///// ListView 긡긚긣걁믁돿뢇맫빁궻귒걂
	//typedef ListView::ColumnInfo Column;
	//typedef ListView::ItemInfo Item;
	//typedef ListView::Align Align;
	//Column columns[] = {Column(L"?뽞괥긶긞??", 0, Align::center, 0, true)
	//				  , Column(L"?뽞괦긶긞??", 0, Align::center, 1, true)
	//				  , Column(L"?뽞괧긶긞??", 0, Align::left  , 2, false)};
	//wstring texts[] = {L"?뽞괥괥", L"?뽞괥괦", L"?뽞괥괧"};
	//Item items[] = {Item(texts, 0)
	//			  , Item(L"?뽞괦", 1)
	//			  , Item(L"?뽞괧", 2)
	//			  , Item(L"?뽞괨", 2)
	//			  , Item(L"?뽞괪", 2)
	//			  , Item(L"?뽞괫", 2)
	//			  , Item(L"?뽞괮", 2)
	//			  };
	////ListView c(frame, 0, 0, 0, 0, columns, items, ListView::Style::details);
	//ListView c(frame, 0, 0, 400, 200, ListView::Style::details, ListView::Options::checkBoxes);
	//c.columnInfos(columns);
	//ImageList images(Icon::smallSize());
	//images.add(Icon::application());
	//images.add(Icon::asterisk());
	//images.add(Icon::error());
	//c.smallImageList(images);
	//c.itemInfos(items);
	////c.resize();
	//c.fullRowSelect(true);
	//c.itemTipEnabled(true);
	//auto toolTip = c.itemTip();
	//toolTip.popDelay(500);
	//toolTip.textColor(Color::red());
	////c.backColor(Color::green());
	//c.onItemChanged() = [&] (ListView::ItemChanged& e) {
	//	if (e.newState() != e.oldState()) {
	//		Debug::writeLine(e.item().text() + L" check changed " + e.newState());
	//	}
	//};
	//ImageList list(Icon::largeSize());
	//list.add(Icon::error());
	//DragDrop::Source source(c);
	//c.onItemDrag() = [&] (ListView::ItemDrag& e) {
	//	source.doDragDrop(e.item().text(), DragDrop::Effect::move//, list, 0);
	//		, c.smallImageList(), e.item().imageIndex(), 8, 8);
	//};
	//c[0].userData(String(L"긡긚긣궬귊궏"));
	////c.clear();

	//Font font2;
	//c.onItemPrePaint() = [&] (ListView::ItemPrePaint& e) {
	//	Debug::writeLine(String() + e.item().index() + L":" + e.columnIndex() + L":" + e.focused());
	//	if (!e.sender().enabled()) {
	//		e.backColor(Color::gray());
	//	} else if (e.focused()) {
	//		//e.focused(false);
	//		e.selected(false);
	//		//if (e.columnIndex() == 0) {
	//			e.backColor(Color::blue());
	//		//} else {
	//		//	e.backColor(Color::yellow());
	//		//}
	//	} else {
	//		e.backColor(Color::yellow());
	//	}
	//	switch (e.columnIndex()) {
	//		case 0 : e.textColor(Color::green()); break;
	//		case 1 : e.textColor(Color::cyan()); break;
	//		case 2 : e.textColor(Color::black()); break;
	//	}
	//};
	//c.onItemTipPopup() = [&] (ListView::ItemTipPopup& e) {
	//	e.setText(e.item().text());
	//};
	//c.onHeaderClick() = [&] (ListView::HeaderClick& ) {
	//	Debug::writeLine(L"onHeaderClick");
	//};
	//c.onItemChanging() = [&] (ListView::ItemChanging& e) {
	//	Debug::writeLine(L"onItemChanging");
	//	if (e.item().index() == 1) {
	//		e.cancel(true);
	//	}
	//};
	//c.onItemChanged() = [&] (ListView::ItemChanged& e) {
	//	Debug::writeLine(L"onItemChanged");
	//};
	//c.onItemClick() = [&] (ListView::ItemClick& e) {
	//	Debug::writeLine(String() + L"onItemClick;" + e.item().index());
	//};
	//c.onItemRightClick() = [&] (ListView::ItemRightClick& e) {
	//	Debug::writeLine(String() + L"onItemRightClick;" + e.item().index());
	//};

	//Button leftArrange(frame, 0, 0, 0, 0, L"leftArrange", [&] (Button::Event& ) {
	//	c.leftArrange(!c.leftArrange());
	//});
	//Button rightImage(frame, 0, 0, 0, 0, L"rightImage", [&] (Button::Event& ) {
	//	c.getColumn(1).rightImage(!c.getColumn(1).rightImage());
	//});
	//Button columnImageIndex(frame, 0, 0, 0, 0, L"columnImageIndex", [&] (Button::Event& ) {
	//	auto column = c.getColumn(1);
	//	column.imageIndex(column.imageIndex() != -1 ? -1 : 2);
	//});
	//Button columnSelected(frame, 0, 0, 0, 0, L"columnSelected", [&] (Button::Event& ) {
	//	auto column = c.getColumn(1);
	//	column.selected(!column.selected());
	//});
	//Button columnWidth(frame, 0, 0, 0, 0, L"columnWidth", [&] (Button::Event& ) {
	//	auto column = c.getColumn(1);
	//	column.width(column.width() * 2);
	//});
	//Button cut(frame, 0, 0, 0, 0, L"cut", [&] (Button::Event& ) {
	//	c[0].cut(!c[0].cut());
	//});
	//Button checked(frame, 0, 0, 0, 0, L"checked", [&] (Button::Event& ) {
	//	c[0].checked(!c[0].checked());
	//});
	//Button ensureVisible(frame, 0, 0, 0, 0, L"ensureVisible", [&] (Button::Event& ) {
	//	c[1].ensureVisible();
	//});
	//Button getBounds(frame, 0, 0, 0, 0, L"getBounds", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c[1].getBounds());
	//	MsgBox::show(String() + c[1].getBounds(ListView::ItemPortion::text));
	//});
	//Button getBounds2(frame, 0, 0, 0, 0, L"getBounds2", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c[1].getSubBounds(1));
	//});
	//Button highlight(frame, 0, 0, 0, 0, L"highlight", [&] (Button::Event& ) {
	//	c[0].highlight(!c[0].highlight());
	//});
	//Button selected(frame, 0, 0, 0, 0, L"selected", [&] (Button::Event& ) {
	//	c[0].selected(!c[0].selected());
	//});
	//Button beginEdit(frame, 0, 0, 0, 0, L"beginEdit", [&] (Button::Event& ) {
	//	c.beginEdit(1);
	//});
	//Button brush(frame, 0, 0, 0, 0, L"brush", [&] (Button::Event& ) {
	//	if (!c.brush()) {
	//		c.brush(frameBrush);
	//		c.brushOrigin(-c.position() - c.clientOrigin());
	//	} else if (c.brush() == frameBrush) {
	//		c.brush(Brush::control());
	//	} else {
	//		c.brush(nullptr);
	//	}
	//});
	//Button checkBoxes(frame, 0, 0, 0, 0, L"checkBoxes", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	c = ListView(*c.parent(), b.x, b.y, b.width, b.height, c.columnInfos(), c.itemInfos(), c.style()
	//		, ListView::Options(c.options() ^ ListView::Options::checkBoxes));
	//	c.zOrder(-1);
	//});
	//Button fullRowSelect(frame, 0, 0, 0, 0, L"fullRowSelect", [&] (Button::Event& ) {
	//	c.fullRowSelect(!c.fullRowSelect());
	//});
	//Button noHeaderClick(frame, 0, 0, 0, 0, L"noHeaderClick", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	c = ListView(*c.parent(), b.x, b.y, b.width, b.height, c.columnInfos(), c.itemInfos(), c.style()
	//		, ListView::Options(c.options() ^ ListView::Options::noHeaderClick));
	//	c.zOrder(-1);
	//});
	//Button iconSpacing(frame, 0, 0, 0, 0, L"iconSpacing", [&] (Button::Event& ) {
	//	auto old = c.iconSpacing();
	//	c.iconSpacing(Size(100, 100));
	//	auto newi = c.iconSpacing();
	//	newi = newi;
	//});
	//Button iconSpacing2(frame, 0, 0, 0, 0, L"iconSpacing2", [&] (Button::Event& ) {
	//	c.iconSpacing(Size(-1, -1));
	//});


	//TrackBar bar(frame, 0, 0, 0, 0, 0, 100, TrackBar::Options::valueTip);
	//Font font(toolTip.font(), 40);
	//{
	//auto tip2 = bar.valueTip();
	//tip2.textColor(Color::red());
	//tip2.font(font);
	//}
	//c.textEditable(true);
	//Edit cedit;
	//c.onTextEditing() = [&] (ListView::TextEditing& e) {
	//	cedit = e.edit();
	//	cedit.textColor(Color::red());
	//	cedit.maxLength(5);
	//	cedit.onKeyPress() = [&] (Control::KeyPress& e) {
	//		if (e.charCode() == L'a') {
	//			c.endEdit();
	//		}
	//	};
	//};
	//c.onTextEdited() = [&] (ListView::TextEdited& e) {
	//	cedit = Edit();
	//};





	///// ToolTip 긡긚긣
	//frame.onPaint() = [&] (Frame::Paint& e) {
	//	typedef Graphics::TextFormat TF;
	//	auto& g = e.graphics();
	//	g.brush(frameBrush);
	//	g.clear();

	//	g.pen(Pen::black());
	//	g.drawRectangle(10, 200, 300, 300);
	//	g.drawRectangle(400, 200, 300, 300);
	//};
	//ToolTip c(frame);
	//ToolTip c2(frame);
	//c.onLinkClick() = [&] (ToolTip::Event& ) {
	//	MsgBox::show(L"link clicked");;
	//};
	//c2.set(frame, 1, Rectangle(10, 200, 300, 300));
	//c2.set(frame, 2, Rectangle(400, 200, 300, 300));
	//c2.onGetText() = [&] (ToolTip::GetText& e) {
	//	switch (e.id()) {
	//		case 1 : e.text(L"괦붥뽞궬궓갏"); break;
	//		case 2 : e.text(L"괧붥뽞궬궓갏"); break;
	//	}
	//};
	//c2.onPop() = [&] (ToolTip::Pop& e) {
	//	switch (e.id()) {
	//		case 1 : Debug::writeLine(L"괦붥뽞pop궬궓갏"); break;
	//		case 2 : Debug::writeLine(L"괧붥뽞pop궬궓갏"); break;
	//	}
	//};
	//c2.onPopup() = [&] (ToolTip::Popup& e) {
	//	e.position(e.position() + Point(100, 0));
	//	switch (e.id()) {
	//		case 1 : Debug::writeLine(L"괦붥뽞show궬궓갏"); break;
	//		case 2 : Debug::writeLine(L"괧붥뽞show궬궓갏"); break;
	//	}
	//	//e.position(0, 0);
	//	//e.position(e.position().x, e.position().y - 300);
	//};
	//TrackBar trackBar(frame, 0, 0, 0, 0, 0, 100);
	//Button button(frame, 0, 0, 0, 0, L"button", [&] (Button::Event& ) {
	//	c.popup();
	//	//MsgBox::show(L"pushed");
	//});
	//Edit text(frame, 0, 0, 0, 0, 15, 1);
	//text.text(L"test Text");
	//Edit delay(frame, 0, 0, 0, 0, 4, 1);
	//delay.text(L"1000");
	//Button initialDelay(frame, 0, 0, 0, 0, L"initialDelay", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.initialDelay());
	//	c.initialDelay(to<int>(delay.text()));
	//});
	//Button popDelay(frame, 0, 0, 0, 0, L"popDelay", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.popDelay());
	//	c.popDelay(to<int>(delay.text()));
	//});
	//Button reshowDelay(frame, 0, 0, 0, 0, L"reshowDelay", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.reshowDelay());
	//	c.reshowDelay(to<int>(delay.text()));
	//});
	//Button activate(frame, 0, 0, 0, 0, L"activate", [&] (Button::Event& ) {
	//	static bool flag = true;
	//	flag = !flag;
	//	c.activate(flag);
	//});
	//Button alwaysPopup(frame, 0, 0, 0, 0, L"alwaysPopup", [&] (Button::Event& ) {
	//	c.alwaysPopup(!c.alwaysPopup());
	//});
	//Button animate(frame, 0, 0, 0, 0, L"animate", [&] (Button::Event& ) {
	//	c.animate(!c.animate());
	//});
	//Button balloon(frame, 0, 0, 0, 0, L"balloon", [&] (Button::Event& ) {
	//	c.balloon(!c.balloon());
	//});
	//Button boundsFromTextBounds(frame, 0, 0, 0, 0, L"boundsFromTextBounds", [&] (Button::Event& ) {
	//	Graphics g(text);
	//	auto size = g.measureText(text.text());
	//	MsgBox::show(String() + c.boundsFromTextBounds(Rectangle(Point(0, 0), size)) + L":" + size);
	//});
	//Button clear(frame, 0, 0, 0, 0, L"clear", [&] (Button::Event& ) {
	//	c.clear();
	//});
	//Button closeButton(frame, 0, 0, 0, 0, L"closeButton", [&] (Button::Event& ) {
	//	c.closeButton(!c.closeButton());
	//});
	//Button fade(frame, 0, 0, 0, 0, L"fade", [&] (Button::Event& ) {
	//	c.fade(!c.fade());
	//});
	//Font font0 = c.font().clone();
	//Font font1(c.font(), 40);
	//Button font(frame, 0, 0, 0, 0, L"font", [&] (Button::Event& ) {
	//	c.font(c.font() == font1 ? (Font::HFONT)font0 : (Font::HFONT)font1);
	//});
	//Button get(frame, 0, 0, 0, 0, L"get", [&] (Button::Event& ) {
	//	MsgBox::show(String(L"걏") + c.get(button) + L"걐");
	//});
	//Button maxTextWidth(frame, 0, 0, 0, 0, L"maxTextWidth", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.maxTextWidth());
	//	c.maxTextWidth(to<int>(delay.text()));
	//});
	//Button noPrefix(frame, 0, 0, 0, 0, L"noPrefix", [&] (Button::Event& ) {
	//	c.noPrefix(!c.noPrefix());
	//});
	//Panel panel0(frame, 0, 0, 0, 600);
	//LineLayout layout2(panel0, true);
	//CheckBox centerCheck(panel0, 0, 0, 0, 0, L"center");
	//CheckBox trackCheck(panel0, 0, 0, 0, 0, L"track");
	//CheckBox absoluteCheck(panel0, 0, 0, 0, 0, L"absolute");
	//CheckBox transparentCheck(panel0, 0, 0, 0, 0, L"transparent");
	//CheckBox parseLinksCheck(panel0, 0, 0, 0, 0, L"parseLinks");
	//transparentCheck.checked(true);
	//auto size = panel0.size();
	//layout2.perform();
	//panel0.resize();
	//Button set(frame, 0, 0, 0, 0, L"set", [&] (Button::Event& ) {
	//	typedef ToolTip::Options Op;
	//	Op options = Op::none;
	//	if (centerCheck.checked()) { options |= Op::center; }
	//	if (trackCheck.checked()) { options |= Op::track; }
	//	if (absoluteCheck.checked()) { options |= Op::absolute; }
	//	if (transparentCheck.checked()) { options |= Op::transparent; }
	//	if (parseLinksCheck.checked()) { options |= Op::parseLinks; }
	//	if (text.text().empty()) {
	//		c.set(button, options);
	//	} else {
	//		c.set(button, text.text(), options);
	//	}
	//});
	//c.onGetText() = [&] (ToolTip::GetText& e) {
	//	e.text(text.text());
	//};
	//Button pop(frame, 0, 0, 0, 0, L"pop", [&] (Button::Event& ) {
	//	c.pop();
	//});
	//Button reset(frame, 0, 0, 0, 0, L"reset", [&] (Button::Event& ) {
	//	c.reset(button);
	//});
	//Button reset2(frame, 0, 0, 0, 0, L"reset2", [&] (Button::Event& ) {
	//	c2.reset(frame, 2);
	//});
	//Button resetDelay(frame, 0, 0, 0, 0, L"resetDelay", [&] (Button::Event& ) {
	//	c.resetDelay();
	//});
	//Button setDelay(frame, 0, 0, 0, 0, L"setDelay", [&] (Button::Event& ) {
	//	c.setDelay(to<int>(delay.text()));
	//});
	//Button textColor(frame, 0, 0, 0, 0, L"textColor", [&] (Button::Event& ) {
	//	c.textColor(c.textColor() == Color::controlText() ? Color::red() : Color::controlText());
	//});
	//Button textFormat(frame, 0, 0, 0, 0, L"textFormat", [&] (Button::Event& ) {
	//	c.textFormat(c.textFormat() == Graphics::TextFormat::none ? Graphics::TextFormat::right : Graphics::TextFormat::none);
	//});
	//Icon titleIcon1;
	//Button title(frame, 0, 0, 0, 0, L"title", [&] (Button::Event& ) {
	//	c.title(text.text(), titleIcon1);
	//});
	//Button titleIcon(frame, 0, 0, 0, 0, L"titleIcon", [&] (Button::Event& ) {
	//	titleIcon1 = titleIcon1 ? Icon() : Icon::exclamation();
	//	c.title(c.title(), titleIcon1);
	//});
	//Button track(frame, 0, 0, 0, 0, L"track", [&] (Button::Event& ) {
	//	c.track(button, Mouse::position());
	//});
	//frame.onMouseMove() = [&] (Control::MouseMove& e) {
	//	c.trackPosition(frame.clientToScreen(e.position()));
	//};
	//Button update(frame, 0, 0, 0, 0, L"update", [&] (Button::Event& ) {
	//	c.update();
	//});
	//Button visible(frame, 0, 0, 0, 0, L"visible", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.visible());
	//});

	////c.maxTextWidth(5000);
	//c.set(button, L"&긟?깑\t?긞긵궬궓");
	//c.set(trackBar, L"궓??????귪");
	////c.textFormat(Graphics::TextFormat::expandTabs);




	//// ComboBox 긡긚긣
	//const wchar_t* strings[] = {L"\t0궇궋궎궑궓", L"012345678궔궖궘궚궞", L"2궠궢궥궧궩", L"3궠궢궥궧궩궇궓궓궓궓궓aaaaaa", L"4궠궢궥궧궩", L"5궠궢궥궧궩", L"6궠궢궥궧궩"};
	//ComboBox cc(frame, 0, 0, 0, 0, strings, ComboBox::Style::dropDown);
	//ComboBox c(move(cc));
	//cc = move(c);
	//c = move(cc);

	//Brush brush2(Color::green());
	//auto& list = c.list();
	//auto setter = [&] () {
	//	if (c.style() != ComboBox::Style::dropDownList) {
	//		auto& edit = c.edit();
	//		edit.textColor(Color::red());
	//		edit.brush(brush2);
	//		edit.onTextChange() = [&] (Edit::TextChange& ) { Debug::writeLine("onTextChange"); };
	//		edit.onPaste() = [&] (Edit::Paste& ) { Debug::writeLine("onPaste"); };
	//	}
	//	list.textColor(Color::red());
	//	list.brush(brush2);
	//	list.size(list.size().width, list.size().height + list.itemHeight() * 3);
	//	list.onSelect() = [&] (ListBox::Event& ) { Debug::writeLine(L"onListSelect"); };
	//	ComboBox& ccc = c;
	//	list.onMouseDown() = [&] (Control::Event& ) { Debug::writeLine(String(L"boundsWithDropDown:") + ccc.boundsWithDropDown()); };

	//	c.onCloseUp() = [&] (ComboBox::Event& ) { Debug::writeLine(L"onCloseUp"); };
	//	c.onDropDown() = [&] (ComboBox::Event& ) { Debug::writeLine(L"onDropDown"); };
	//	c.onSelect() = [&] (ComboBox::Event& ) { Debug::writeLine(L"onSelect"); };
	//	c.onSelectCancel() = [&] (ComboBox::Event& ) { Debug::writeLine(L"onSelectCanceled"); };
	//	c.onSelectEnd() = [&] (ComboBox::Event& ) { Debug::writeLine(L"onSelectEnd"); };
	//	c.onMouseDown() = [&] (Control::MouseDown& ) {
	//		Debug::writeLine(String(L"buttonIs:") + c.dropDownButtonIsPushed());
	//		Debug::writeLine(String(L"dropDowned:") + c.dropDowned());
	//	};
	//};

	//Button simple(frame, 0, 0, 0, 0, L"simple", [&] (Button::Event& ) {
	//	c = ComboBox(frame, 0, 0, 0, 0, c.list().items(), ComboBox::Style::simple, c.options());
	//	c.zOrder(-1);
	//	layout.perform();
	//	setter();
	//});
	//Button dropDownList(frame, 0, 0, 0, 0, L"dropDownList", [&] (Button::Event& ) {
	//	c = ComboBox(frame, 0, 0, 0, 0, c.list().items(), ComboBox::Style::dropDownList, c.options());
	//	c.zOrder(-1);
	//	layout.perform();
	//	setter();
	//});
	//Button autoHScroll(frame, 0, 0, 0, 0, L"autoHScroll", [&] (Button::Event& ) {
	//	c = ComboBox(frame, 0, 0, 0, 0, c.list().items(), c.style(), ComboBox::Options(c.options() ^ ComboBox::Options::noHScroll));
	//	c.zOrder(-1);
	//	layout.perform();
	//	setter();
	//});
	//Button boundsWithDropDown(frame, 0, 0, 0, 0, L"boundsWithDropDown", [&] (Button::Event& ) {
	//	Debug::writeLine(String(L"boundsWithDropDown:") + c.boundsWithDropDown());
	//});
	//Button dropDownButtonBounds(frame, 0, 0, 0, 0, L"dropDownButtonBounds", [&] (Button::Event& ) {
	//	Debug::writeLine(String(L"dropDownButtonBounds:") + c.dropDownButtonBounds());
	//});
	//Button dropDowned(frame, 0, 0, 0, 0, L"dropDowned", [&] (Button::Event& ) {
	//	c.dropDowned(!c.dropDowned());
	//});
	//Edit dropDownScrollWidth1(frame, 0, 0, 0, 0, 4);
	//dropDownScrollWidth1.text(String() + c.dropDownScrollWidth());
	//Button dropDownScrollWidth(frame, 0, 0, 0, 0, L"dropDownScrollWidth", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.dropDownScrollWidth());
	//});
	//Button dropDownScrollWidthSet(frame, 0, 0, 0, 0, L"dropDownScrollWidthSet", [&] (Button::Event& ) {
	//	c.dropDownScrollWidth(to<int>(dropDownScrollWidth1.text()));
	//});
	//Button dropDownWidth(frame, 0, 0, 0, 0, L"dropDownWidth", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.dropDownWidth());
	//});
	//Button dropDownWidthSet(frame, 0, 0, 0, 0, L"dropDownWidthSet", [&] (Button::Event& ) {
	//	c.dropDownWidth(to<int>(dropDownScrollWidth1.text()));
	//});
	//Button extendedUI(frame, 0, 0, 0, 0, L"extendedUI", [&] (Button::Event& ) {
	//	c.extendedUI(!c.extendedUI());
	//});
	//Button integralHeight(frame, 0, 0, 0, 0, L"integralHeight", [&] (Button::Event& ) {
	//	c = ComboBox(frame, 0, 0, 0, 0, c.list().items(), c.style(), ComboBox::Options(c.options() ^ ComboBox::Options::noIntegralHeight));
	//	c.zOrder(-1);
	//	layout.perform();
	//	setter();
	//});
	//Button editHeight(frame, 0, 0, 0, 0, L"editHeight", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.editHeight());
	//});
	//Button editHeightSet(frame, 0, 0, 0, 0, L"editHeightSet", [&] (Button::Event& ) {
	//	c.editHeight(to<int>(dropDownScrollWidth1.text()));
	//});
	//Button maxVisibleItemCount(frame, 0, 0, 0, 0, L"maxVisibleItemCount", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.maxVisibleItemCount());
	//});
	//Button maxVisibleItemCountSet(frame, 0, 0, 0, 0, L"maxVisibleItemCountSet", [&] (Button::Event& ) {
	//	c.maxVisibleItemCount(to<int>(dropDownScrollWidth1.text()));
	//});
	//Button selectedIndex(frame, 0, 0, 0, 0, L"selectedIndex", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.selectedIndex());
	//});
	//Button selectedIndexSet(frame, 0, 0, 0, 0, L"selectedIndexSet", [&] (Button::Event& ) {
	//	c.selectedIndex(to<int>(dropDownScrollWidth1.text()));
	//});

	//setter();

	////ComboBox c(move(cc));
	////Edit& c = cc.edit();
	////ListBox& c = cc.list();


	////list.onMouseDown() = [&] (Control::MouseDown& e) {
	////	Debug::writeLine(String() + list.getIndexAt(e.position()));
	////};
	////list.setItemData(0, String(L"궇궯궼?귪"));
	////Edit item(frame, 0, 0, 0, 0, 20);
	////item.text(L"?뽞믁돿괥");
	////item.brush(Brush::black());
	////item.textColor(Color::yellow());
	////Edit index(frame, 0, 0, 0, 0, 3);
	////index.text(L"0");
	////index.align(Edit::Align::right);
	////auto onSelect = [&] (ListBox::Event& ) {
	////	Debug::writeLine(L"select");
	////	index.text(String() + list.focusedIndex());
	////};
	////list.onSelect() = onSelect;
	////Button add(frame, 0, 0, 0, 0, L"add", [&] (Button::Event& ) {
	////	list.add(item.text());
	////});
	////Brush addBrush(Color::red());
	////add.brush(addBrush);
	////Button anchorIndex(frame, 0, 0, 0, 0, L"anchorIndex", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.anchorIndex());
	////});
	////Button anchorIndexSet(frame, 0, 0, 0, 0, L"anchorIndexSet", [&] (Button::Event& ) {
	////	list.anchorIndex(to<int>(index.text()));
	////});
	////Button brushs(frame, 0, 0, 0, 0, L"brush", [&] (Button::Event& ) {
	////	list.brushOrigin(-list.position());
	////	list.brush(!list.brush() ? (Brush::HBRUSH)brush : nullptr);
	////});
	////Button clear(frame, 0, 0, 0, 0, L"clear", [&] (Button::Event& ) {
	////	list.clear();
	////});
	////Button clearSelected(frame, 0, 0, 0, 0, L"clearSelected", [&] (Button::Event& ) {
	////	list.clearSelected();
	////});
	////Button erase(frame, 0, 0, 0, 0, L"erase", [&] (Button::Event& ) {
	////	list.erase(to<int>(index.text()));
	////});
	////Button insert(frame, 0, 0, 0, 0, L"insert", [&] (Button::Event& ) {
	////	list.insert(to<int>(index.text()), item.text());
	////});
	////Button count(frame, 0, 0, 0, 0, L"count", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.count());
	////});
	////Button countPerPage(frame, 0, 0, 0, 0, L"countPerPage", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.countPerPage());
	////});
	////int edge0 = (int)list.edge();
	////Button edge(frame, frame.lastBounds().x, frame.lastBounds().bottom() + 50, 0, 0, L"edge", [&] (Button::Event& ) {
	////	++edge0;
	////	if (3 < edge0) {
	////		edge0 = 0;
	////	}
	////	list.edge(Control::Edge(edge0));
	////	//list.resize();
	////});
	////Button firstVisibleIndex(frame, 0, 0, 0, 0, L"firstVisibleIndex", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.firstVisibleIndex());
	////});
	////Button firstVisibleIndexSet(frame, 0, 0, 0, 0, L"firstVisibleIndexSet", [&] (Button::Event& ) {
	////	list.firstVisibleIndex(to<int>(index.text()));
	////});
	////Button focusedIndex(frame, 0, 0, 0, 0, L"focusedIndex", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.focusedIndex());
	////});
	////Button focusedIndexSet(frame, 0, 0, 0, 0, L"focusedIndexSet", [&] (Button::Event& ) {
	////	list.focusedIndex(to<int>(index.text()));
	////});
	////Button getItem(frame, 0, 0, 0, 0, L"getItem", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.getItem(to<int>(index.text())));
	////});
	////Button getItemBounds(frame, 0, 0, 0, 0, L"getItemBounds", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.getItemBounds(to<int>(index.text())));
	////});
	////Button getItemData(frame, 0, 0, 0, 0, L"getItemData", [&] (Button::Event& ) {
	////	auto i = to<int>(index.text());
	////	if (list.getItemDataIsEmpty(i)) {
	////		MsgBox::show(String() + L"null");
	////	} else {
	////		MsgBox::show(String() + any_cast<String>(list.getItemData<String>(i)));
	////	}
	////});
	////Button getPreferredSize(frame, 0, 0, 0, 0, L"getPreferredSize", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.getPreferredSize() + L" : " + list.size());
	////});
	////Button integralHeight2(frame, 0, 0, 0, 0, L"integralHeight", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.integralHeight());
	////});
	////Button isSelected(frame, 0, 0, 0, 0, L"isSelected", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.isSelected(to<int>(index.text())));
	////});
	////Button isSelectedSet(frame, 0, 0, 0, 0, L"isSelectedSet", [&] (Button::Event& ) {
	////	list.isSelected(to<int>(index.text()), !list.isSelected(to<int>(index.text())));
	////});
	////Edit itemHeight1(frame, 0, 0, 0, 0, 4);
	////itemHeight1.text(String() + list.itemHeight());
	////Button itemHeight(frame, 0, 0, 0, 0, L"itemHeight", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.itemHeight());
	////});
	////Button itemHeightSet(frame, 0, 0, 0, 0, L"itemHeightSet", [&] (Button::Event& ) {
	////	list.itemHeight(to<int>(itemHeight1.text()));
	////});
	////Button maxItemWidth(frame, 0, 0, 0, 0, L"maxItemWidth", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.maxItemWidth() + L" : " + list.size().width);
	////});
	//////Button multiColumn(frame, 0, 0, 0, 0, L"multiColumn", [&] (Button::Event& ) {
	//////	auto b = list.bounds();
	//////	int width = list.multiColumn() ? list.maxItemWidth() : list.maxItemWidth() * 3;
	//////	list = ListBox(frame, b.x, b.y, width, b.height, list.items(), list.selectMode(), list.scrollMode(), !list.multiColumn(), list.integralHeight());
	//////	list.columnWidth(list.maxItemWidth());
	//////	list.onSelect() = onSelect;
	//////	list.zOrder(-1);
	//////	layout.perform();
	//////});
	////Edit select1(frame, 0, 0, 0, 0, 4);
	////select1.text(String() + 0);
	////Edit select2(frame, 0, 0, 0, 0, 4);
	////select2.text(String() + 0);
	////Button select(frame, 0, 0, 0, 0, L"select", [&] (Button::Event& ) {
	////	list.select(to<int>(select1.text()), to<int>(select2.text()), !list.isSelected(to<int>(select1.text())));
	////});
	////Button selectedIndex2(frame, 0, 0, 0, 0, L"selectedIndex", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.selectedIndex());
	////});
	////Button selectedIndexSet2(frame, 0, 0, 0, 0, L"selectedIndexSet", [&] (Button::Event& ) {
	////	list.selectedIndex(to<int>(index.text()));
	////});
	////Button selectedIndices(frame, 0, 0, 0, 0, L"selectedIndices", [&] (Button::Event& ) {
	////	auto is = list.selectedIndices();
	////	MsgBox::show(String() + is.size());
	////	list.clearSelected();
	////	list.selectedIndices(is);
	////});
	////Button scrollWidth(frame, 0, 0, 0, 0, L"scrollWidth", [&] (Button::Event& ) {
	////	list.scrollWidth(to<int>(itemHeight1.text()));
	////});
	////Button selectMode(frame, 0, 0, 0, 0, L"selectMode", [&] (Button::Event& ) {
	////	MsgBox::show(String() + (int)list.selectMode());
	////});
	//////Button selectModeSet(frame, 0, 0, 0, 0, L"selectModeSet", [&] (Button::Event& ) {
	//////	typedef ListBox::SelectMode SM;
	//////	SM mode = SM::none;
	//////	switch (to<int>(index.text())) {
	//////		case 1 : mode = SM::one; break;
	//////		case 2 : mode = SM::multiple; break;
	//////		case 3 : mode = SM::multipleExtended; break;
	//////	}
	//////	auto b = list.bounds();
	//////	list = ListBox(frame, b.x, b.y, b.width, b.height, list.items(), mode, list.scrollMode(), list.multiColumn(), list.integralHeight());
	//////	list.onSelect() = onSelect;
	//////	list.zOrder(-1);
	//////	layout.perform();
	//////});
	////Button tabWidth(frame, 0, 0, 0, 0, L"tabWidth", [&] (Button::Event& ) {
	////	MsgBox::show(String() + list.tabWidth());
	////});
	////Button tabWidthSet(frame, 0, 0, 0, 0, L"tabWidthSet", [&] (Button::Event& ) {
	////	list.tabWidth(to<int>(index.text()));
	////});
	////Button textColor(frame, 0, 0, 0, 0, L"textColor", [&] (Button::Event& ) {
	////	list.textColor(list.textColor() == Color::controlText() ? Color::red() : Color::controlText());
	////});


	////Font font30(c.font(), 30, Font::Style::regular);
	////c.font(font30);
	////c.edge(Edit::Edge::none);
	////c.preferredCharSize(30, 3);
	////c.hScrollBar(true);
	////c.vScrollBar(true);
	////c.margin(0, 0, 0, 0);
	////c.margin(10, 15, 10, 15);
	////c.margin(10, 0, 20, 0);
	////c.resize();
	////c.onShortcutKey() = [&] (Edit::ShortcutKey& e) {
	////	switch (e.shortcut()) {
	////		case Key::tab : e.isInputKey(true); break;
	////		case Key::escape : e.isInputKey(true); break;
	////		case Key::enter : e.isInputKey(true); break;
	////	}
	////};

	//std::wregex cRegex(L"-?[0-9]*\\.?[0-9]*");
	////std::wregex cRegex(L"[0-9a-zA-Z궇-귪]*");
	//c.edit().onTextChange() = [&] (Edit::TextChange& e) {
	//	//StringBuffer buffer(e.newText());
	//	//for (auto i = buffer.begin(), end = buffer.end(); i != end; ++i) {
	//	//	if (L'궆' <= *i && *i <= L'귪') {
	//	//		*i += L'?' - L'궆';
	//	//	}
	//	//}
	//	//e.newText(buffer);

	//	if (!std::regex_match(e.newText().c_str(), cRegex)) {
	//		e.cancel(true);
	//	}
	//};
	////c.edit().onKeyPress() = [&] (Edit::KeyPress& e) {
	////	if (L'궆' <= e.charCode() && e.charCode() <= L'귪') {
	////		e.charCode(e.charCode() + L'?' - L'궆');
	////	}
	////};
	////c.edit().onPaste() = [&] (Edit::Paste& e) {
	////	e.cancel(true);
	////};
	////c.onPrePaint() = [&] (Edit::PrePaint& e) {
	////	e.brush(brush);
	////	e.brushOrigin(-e.sender().position());
	////	e.textColor(Color::red());
	////};
	//c.edit().onMouseDown() = [&] (Edit::MouseDown& e) {
	//	if (e.button() == Mouse::mButton) {
	//		Debug::writeLine(String() + c.edit().getIndexAt(c.edit().screenToClient(Mouse::position())));
	//	}
	//};

	////String items[] = {L"귺귽긂", L"귺귺긂", L"012", L"abc", L"abd", L"abcdef"};
	////AutoComplete acc(c.edit(), AutoComplete::Mode::appendAndSuggest, items);
	////AutoComplete acc(c.edit(), AutoComplete::Mode::appendAndSuggest, AutoComplete::SystemItems::fileSystem);


	//int edge = (int)c.edit().edge();
	//Button edgeButton(frame, frame.lastBounds().x, frame.lastBounds().bottom() + 50, 0, 0, L"edge", [&] (Button::Event& ) {
	//	++edge;
	//	if (3 < edge) {
	//		edge = 0;
	//	}
	//	c.edit().edge(Control::Edge(edge));
	//	c.edit().resize();
	//});
	//int align = (int)c.edit().align();
	//Button textAlignButton(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"align", [&] (Button::Event& ) {
	//	++align;
	//	if (2 < align) {
	//		align = 0;
	//	}
	//	c.edit().align(Edit::Align(align));
	//});
	//Button hScrollBar(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"hScrollBar", [&] (Button::Event& ) {
	//	c.edit().hScrollBar(!c.edit().hScrollBar());
	//	c.edit().resize();
	//});
	//Button vScrollBar(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"vScrollBar", [&] (Button::Event& ) {
	//	c.edit().vScrollBar(!c.edit().vScrollBar());
	//	c.edit().resize();
	//});
	//Button lowercaseOnly(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"lowercaseOnly", [&] (Button::Event& ) {
	//	c.edit().lowercaseOnly(!c.edit().lowercaseOnly());
	//});
	//Button uppercaseOnly(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"uppercaseOnly", [&] (Button::Event& ) {
	//	c.edit().uppercaseOnly(!c.edit().uppercaseOnly());
	//});
	//Button numberOnly(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"numberOnly", [&] (Button::Event& ) {
	//	c.edit().numberOnly(!c.edit().numberOnly());
	//});
	//Button passwordChar(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"passwordChar", [&] (Button::Event& ) {
	//	c.edit().passwordChar(!c.edit().passwordChar() ? L'*' : L'\0');
	//});
	//Button canUndo(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"canUndo", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.edit().canUndo());
	//});
	//Button clearUndo(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"clearUndo", [&] (Button::Event& ) {
	//	c.edit().clearUndo();
	//});
	//Button copy(frame, frame.firstBounds().x, frame.lastBounds().bottom() + 30, 0, 0, L"copy", [&] (Button::Event& ) {
	//	c.edit().copy();
	//});
	//Button cut(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"cut", [&] (Button::Event& ) {
	//	c.edit().cut();
	//});
	//Button paste(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"paste", [&] (Button::Event& ) {
	//	c.edit().paste();
	//});
	//Button undo(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"undo", [&] (Button::Event& ) {
	//	c.edit().undo();
	//});
	//Button multiline(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"multiline", [&] (Button::Event& ) {
	//	Debug::writeLine(String() + L"multi:" + c.edit().selection().index + L":" + c.edit().selection().length);
	//});
	//Button readOnly(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"readOnly", [&] (Button::Event& ) {
	//	c.edit().readOnly(!c.edit().readOnly());
	//});
	//Button maxLength(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"maxLength", [&] (Button::Event& ) {
	//	c.edit().maxLength(c.edit().maxLength() != 3 ? 3 : 10);
	//});
	////Button autoHScroll2(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"autoHScroll", [&] (Button::Event& ) {
	////	Debug::writeLine(String() + L"autoHScroll:" + c.edit().autoHScroll());
	////});
	////Button autoVScroll(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"autoVScroll", [&] (Button::Event& ) {
	////	Debug::writeLine(String() + L"autoVScroll:" + c.edit().autoVScroll());
	////});
	////Button hideSelection(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"hideSelection", [&] (Button::Event& ) {
	////	Debug::writeLine(String() + L"hide:" + c.edit().hideSelection());
	////});
	//Button firstVisibleLine(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"firstVisibleLine", [&] (Button::Event& ) {
	//	Debug::writeLine(String() + c.edit().firstVisibleLine());
	//});
	//Button cueBanner(frame, frame.firstBounds().x, frame.lastBounds().bottom() + 20, 0, 0, L"cueBanner", [&] (Button::Event& ) {
	//	c.edit().cueBanner(L"룊딖?렑");
	//	assert(c.edit().cueBanner() == L"룊딖?렑");
	//});
	//Button noKeyPress(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"noKeyPress", [&] (Button::Event& ) {
	//	auto s = toFlag(c.edit().imeStatus());
	//	s.set(Edit::ImeStatus::noKeyPress, !s[Edit::ImeStatus::noKeyPress]);
	//	c.edit().imeStatus(s);
	//});
	//Button cancelOnGotFocus(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"cancelOnGotFocus", [&] (Button::Event& ) {
	//	auto s = toFlag(c.edit().imeStatus());
	//	s.set(Edit::ImeStatus::cancelOnGotFocus, !s[Edit::ImeStatus::cancelOnGotFocus]);
	//	c.edit().imeStatus(s);
	//});
	//Button completeOnLostFocus(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"completeOnLostFocus", [&] (Button::Event& ) {
	//	auto s = toFlag(c.edit().imeStatus());
	//	s.set(Edit::ImeStatus::completeOnLostFocus, !s[Edit::ImeStatus::completeOnLostFocus]);
	//	c.edit().imeStatus(s);
	//});
	//Button lineCount(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"lineCount", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.edit().lineCount());
	//});
	//Button caretIndex(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"carentIndex", [&] (Button::Event& ) {
	//	//c.edit().focus();
	//	MsgBox::show(String() + c.edit().caretIndex());
	//});
	//Edit caretSet(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, 1);
	//caretSet.text(L"0");
	//Button caretIndexSet(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"caretIndexSet", [&] (Button::Event& ) {
	//	auto text = caretSet.text();
	//	if (!text.empty()) {
	//		int i = Convert::to<int>(text);
	//		c.edit().caretIndex(i);
	//	}
	//});
	//Button getIndexFromLine(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"getIndexFromLine", [&] (Button::Event& ) {
	//	for (int i = 0; i < c.edit().lineCount(); ++i) {
	//		Debug::writeLine(String() + L"line:" + i + L" = " + c.edit().getIndexFromLine(i));
	//	}
	//});
	//Button getLineFromIndex(frame, frame.firstBounds().x, frame.lastBounds().bottom() + 30, 0, 0, L"getLineFromIndex", [&] (Button::Event& ) {
	//	for (int i = 0; i <= c.edit().length(); ++i) {
	//		Debug::writeLine(String() + L"index:" + i + L" = " + c.edit().getLineFromIndex(i));
	//	}
	//});
	//Button getLineLength(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"getLineLength", [&] (Button::Event& ) {
	//	for (int i = 0; i < c.edit().lineCount(); ++i) {
	//		Debug::writeLine(String() + L"line:" + i + L" = " + c.edit().getLineLength(i));
	//	}
	//});
	//Button getLineText(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"getLineText", [&] (Button::Event& ) {
	//	for (int i = 0; i < c.edit().lineCount(); ++i) {
	//		Debug::writeLine(String() + L"line:" + i + L" = " + c.edit().getLineText(i));
	//	}
	//});
	//Button scrollToCaret(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"scrollToCaret", [&] (Button::Event& ) {
	//	c.edit().scrollToCaret();
	//});
	//Button scrollLineDown(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"scrollLineDown", [&] (Button::Event& ) {
	//	c.edit().scrollLineDown();
	//});
	//Button scrollLineUp(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"scrollLineUp", [&] (Button::Event& ) {
	//	c.edit().scrollLineUp();
	//});
	//Button scrollPageDown(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"scrollPageDown", [&] (Button::Event& ) {
	//	c.edit().scrollPageDown();
	//});
	//Button scrollPageUp(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"scrollPageUp", [&] (Button::Event& ) {
	//	c.edit().scrollPageUp();
	//});
	//Edit scroll1(frame, frame.firstBounds().x, frame.lastBounds().bottom() + 30, 0, 0, 2);
	//Edit scroll2(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, 2);
	//Button scroll(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"scroll", [&] (Button::Event& ) {
	//	int x = 0, y = 0;
	//	if (scroll1.length()) {
	//		x = Convert::to<int>(scroll1.text());
	//	}
	//	if (scroll2.length()) {
	//		y = Convert::to<int>(scroll2.text());
	//	}
	//	c.edit().scrollInto(x, y);
	//});
	//Button selection(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"selection", [&] (Button::Event& ) {
	//	c.edit().selection(1, 3);
	//});
	//Edit tabWidth1(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, 2);
	//tabWidth1.text(L"8");
	//Button tabWidth(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"tabWidth", [&] (Button::Event& ) {
	//	int i = 0;
	//	if (tabWidth1.length()) {
	//		i = Convert::to<int>(tabWidth1.text());
	//	}
	//	c.edit().tabWidth(i);
	//});
	//Button caretVisible(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"caretVisible", [&] (Button::Event& ) {
	//	c.edit().caretVisible(!c.edit().caretVisible());
	//});
	//bool change = false;
	//Bitmap car(6, 12);
	//{
	//	Graphics g(car);
	//	//g.brush(Color::black());
	//	//g.clear();
	//	g.brush(Color::white());
	//	g.drawRectangle(0, 0, 6, 12);
	//}
	//Button changeCaret(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"changeCaret", [&] (Button::Event& ) {
	//	if (change) {
	//		c.edit().changeCaret(1, c.edit().font().height());
	//	} else {
	//		Graphics g(c.edit());
	//		g.font(c.edit().font());
	//		c.edit().changeCaret(g.measureText(L"0"), true);
	//	}
	//	change = !change;
	//});
	//Button changeCaret2(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"changeCaret2", [&] (Button::Event& ) {
	//	if (change) {
	//		c.edit().changeCaret(1, c.edit().font().height());
	//	} else {
	//		c.edit().changeCaret(car);
	//	}
	//	change = !change;
	//});





	//// ListBox 긡긚긣
	//const wchar_t* strings[] = {L"\t0궇궋궎궑궓", L"012345678궔궖궘궚궞", L"2궠궢궥궧궩", L"3궠궢궥궧궩궇궓궓궓궓궓aaaaaa", L"4궠궢궥궧궩", L"5궠궢궥궧궩", L"6궠궢궥궧궩"};
	//ListBox cc(frame, 0, 0, 0, 0, strings, ListBox::SelectMode::multipleExtended, ListBox::Options::scrollBarFixed | ListBox::Options::noVScroll);
	//ListBox c(move(cc));
	//cc = move(c);
	//c = move(cc);
	////c.tabWidth(8);

	//c.onMouseDown() = [&] (Control::MouseDown& e) {
	//	Debug::writeLine(String() + c.getIndexAt(e.position()));
	//};
	//c.setItemData(0, String(L"궇궯궼?귪"));
	//Edit item(frame, 0, 0, 0, 0, 20);
	//item.text(L"?뽞믁돿괥");
	//item.brush(Brush::black());
	//item.textColor(Color::yellow());
	//Edit index(frame, 0, 0, 0, 0, 3);
	//index.text(L"0");
	//index.align(Edit::Align::right);
	//auto onSelect = [&] (ListBox::Event& ) {
	//	Debug::writeLine(L"select");
	//	index.text(String() + c.focusedIndex());
	//};
	//c.onSelect() = onSelect;
	//Button add(frame, 0, 0, 0, 0, L"add", [&] (Button::Event& ) {
	//	c.add(item.text());
	//});
	//Brush addBrush(Color::red());
	//add.brush(addBrush);
	//Button anchorIndex(frame, 0, 0, 0, 0, L"anchorIndex", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.anchorIndex());
	//});
	//Button anchorIndexSet(frame, 0, 0, 0, 0, L"anchorIndexSet", [&] (Button::Event& ) {
	//	c.anchorIndex(to<int>(index.text()));
	//});
	//Button brushs(frame, 0, 0, 0, 0, L"brush", [&] (Button::Event& ) {
	//	c.brushOrigin(-c.position());
	//	c.brush(!c.brush() ? (Brush::HBRUSH)frameBrush : nullptr);
	//});
	//Button clear(frame, 0, 0, 0, 0, L"clear", [&] (Button::Event& ) {
	//	c.clear();
	//});
	//Button clearSelected(frame, 0, 0, 0, 0, L"clearSelected", [&] (Button::Event& ) {
	//	c.clearSelected();
	//});
	//Button erase(frame, 0, 0, 0, 0, L"erase", [&] (Button::Event& ) {
	//	c.erase(to<int>(index.text()));
	//});
	//Button insert(frame, 0, 0, 0, 0, L"insert", [&] (Button::Event& ) {
	//	c.insert(to<int>(index.text()), item.text());
	//});
	//Button count(frame, 0, 0, 0, 0, L"count", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.count());
	//});
	//Button countPerPage(frame, 0, 0, 0, 0, L"countPerPage", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.countPerPage());
	//});
	//int edge0 = (int)c.edge();
	//Button edge(frame, frame.lastBounds().x, frame.lastBounds().bottom() + 50, 0, 0, L"edge", [&] (Button::Event& ) {
	//	++edge0;
	//	if (3 < edge0) {
	//		edge0 = 0;
	//	}
	//	c.edge(Control::Edge(edge0));
	//	//c.resize();
	//});
	//Button firstVisibleIndex(frame, 0, 0, 0, 0, L"firstVisibleIndex", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.firstVisibleIndex());
	//});
	//Button firstVisibleIndexSet(frame, 0, 0, 0, 0, L"firstVisibleIndexSet", [&] (Button::Event& ) {
	//	c.firstVisibleIndex(to<int>(index.text()));
	//});
	//Button focusedIndex(frame, 0, 0, 0, 0, L"focusedIndex", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.focusedIndex());
	//});
	//Button focusedIndexSet(frame, 0, 0, 0, 0, L"focusedIndexSet", [&] (Button::Event& ) {
	//	c.focusedIndex(to<int>(index.text()));
	//});
	//Button getItem(frame, 0, 0, 0, 0, L"getItem", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.getItem(to<int>(index.text())));
	//});
	//Button getItemBounds(frame, 0, 0, 0, 0, L"getItemBounds", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.getItemBounds(to<int>(index.text())));
	//});
	//Button getItemData(frame, 0, 0, 0, 0, L"getItemData", [&] (Button::Event& ) {
	//	auto i = to<int>(index.text());
	//	if (c.getItemDataIsEmpty(i)) {
	//		MsgBox::show(String() + L"null");
	//	} else {
	//		MsgBox::show(String() + any_cast<String>(c.getItemData<String>(i)));
	//	}
	//});
	//Button getPreferredSize(frame, 0, 0, 0, 0, L"getPreferredSize", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.getPreferredSize() + L" : " + c.size());
	//});
	//Button noIntegralHeight(frame, 0, 0, 0, 0, L"noIntegralHeight", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	c = ListBox(*c.parent(), b.x, b.y, b.width, b.height, c.items(), c.selectMode(), ListBox::Options(c.options() ^ ListBox::Options::noIntegralHeight));
	//});
	//Button isSelected(frame, 0, 0, 0, 0, L"isSelected", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.isSelected(to<int>(index.text())));
	//});
	//Button isSelectedSet(frame, 0, 0, 0, 0, L"isSelectedSet", [&] (Button::Event& ) {
	//	c.isSelected(to<int>(index.text()), !c.isSelected(to<int>(index.text())));
	//});
	//Edit itemHeight1(frame, 0, 0, 0, 0, 4);
	//itemHeight1.text(String() + c.itemHeight());
	//Button itemHeight(frame, 0, 0, 0, 0, L"itemHeight", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.itemHeight());
	//});
	//Button itemHeightSet(frame, 0, 0, 0, 0, L"itemHeightSet", [&] (Button::Event& ) {
	//	c.itemHeight(to<int>(itemHeight1.text()));
	//});
	//Button maxItemWidth(frame, 0, 0, 0, 0, L"maxItemWidth", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.maxItemWidth() + L" : " + c.size().width);
	//});
	//Button multiColumn(frame, 0, 0, 0, 0, L"multiColumn", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	int width = c.options() & ListBox::Options::multiColumn ? c.maxItemWidth() : c.maxItemWidth() * 3;
	//	c = ListBox(*c.parent(), b.x, b.y, b.width, b.height, c.items(), c.selectMode(), ListBox::Options(c.options() ^ ListBox::Options::multiColumn));
	//	if (c.options() & ListBox::Options::multiColumn) {
	//		c.columnWidth(c.maxItemWidth());
	//	}
	//	c.onSelect() = onSelect;
	//	c.zOrder(-1);
	//	layout.perform();
	//});
	//Edit select1(frame, 0, 0, 0, 0, 4);
	//select1.text(String() + 0);
	//Edit select2(frame, 0, 0, 0, 0, 4);
	//select2.text(String() + 0);
	//Button select(frame, 0, 0, 0, 0, L"select", [&] (Button::Event& ) {
	//	c.select(to<int>(select1.text()), to<int>(select2.text()), !c.isSelected(to<int>(select1.text())));
	//});
	//Button selectedIndex(frame, 0, 0, 0, 0, L"selectedIndex", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.selectedIndex());
	//});
	//Button selectedIndexSet(frame, 0, 0, 0, 0, L"selectedIndexSet", [&] (Button::Event& ) {
	//	c.selectedIndex(to<int>(index.text()));
	//});
	//Button selectedIndices(frame, 0, 0, 0, 0, L"selectedIndices", [&] (Button::Event& ) {
	//	auto is = c.selectedIndices();
	//	MsgBox::show(String() + is.size());
	//	c.clearSelected();
	//	c.selectedIndices(is);
	//});
	//Button scrollWidth(frame, 0, 0, 0, 0, L"scrollWidth", [&] (Button::Event& ) {
	//	c.scrollWidth(to<int>(itemHeight1.text()));
	//});
	//Button selectMode(frame, 0, 0, 0, 0, L"selectMode", [&] (Button::Event& ) {
	//	MsgBox::show(String() + (int)c.selectMode());
	//});
	//Button selectModeSet(frame, 0, 0, 0, 0, L"selectModeSet", [&] (Button::Event& ) {
	//	typedef ListBox::SelectMode SM;
	//	SM mode = SM::none;
	//	switch (to<int>(index.text())) {
	//		case 1 : mode = SM::one; break;
	//		case 2 : mode = SM::multiple; break;
	//		case 3 : mode = SM::multipleExtended; break;
	//	}
	//	auto b = c.bounds();
	//	c = ListBox(frame, b.x, b.y, b.width, b.height, c.items(), mode, c.options());
	//	c.onSelect() = onSelect;
	//	c.zOrder(-1);
	//	layout.perform();
	//});
	//Button tabWidth(frame, 0, 0, 0, 0, L"tabWidth", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.tabWidth());
	//});
	//Button tabWidthSet(frame, 0, 0, 0, 0, L"tabWidthSet", [&] (Button::Event& ) {
	//	c.tabWidth(to<int>(index.text()));
	//});
	//Button textColor(frame, 0, 0, 0, 0, L"textColor", [&] (Button::Event& ) {
	//	c.textColor(c.textColor() == Color::controlText() ? Color::red() : Color::controlText());
	//});




	//// TrackBar 긡긚긣
	//TrackBar cc(frame, 0, 0, 0, 0, 0, 100, TrackBar::Options::valueTip);
	//TrackBar c(move(cc));
	//cc = move(c);
	//c = move(cc);
	//c.lineSize(5);
	//auto tip = c.valueTip();
	//tip.textColor(Color::red());

	//int start = 0;
	//c.onMouseDown() = [&] (Control::MouseDown& e) {
	//	if (e.lButton() && e.ctrl()) {
	//		start = c.value();
	//		c.selectionBegin(start);
	//		c.selectionEnd(start);
	//	}
	//};
	//c.onMouseMove() = [&] (Control::MouseMove& e) {
	//	if (e.lButton() && e.ctrl()) {
	//		auto value = c.value();
	//		if (value <= start) {
	//			c.selectionBegin(value);
	//			c.selectionEnd(start);
	//		} else {
	//			c.selectionBegin(start);
	//			c.selectionEnd(value);
	//		}
	//	}
	//};

	//Label add1(frame, 0, 0, 0, 0, L"0000");
	//c.onValueChange() = [&] (TrackBar::ValueChange& e) {
	//	add1.text(String() + c.value());
	//};
	//add1.text(String() + c.value());
	//Edit add2(frame, 0, 0, 0, 0, 5);
	//add2.text(L"10");
	//Button value(frame, 0, 0, 0, 0, L"value", [&] (Button::Event& ) {
	//	c.value(to<int>(add2.text()));
	//	add1.text(String() + c.value());
	//});
	//Edit inde(frame, 0, 0, 0, 0, 5);
	//inde.text(L"100");
	//Button maximum(frame, 0, 0, 0, 0, L"maximum", [&] (Button::Event& ) {
	//	c.maximum(to<int>(inde.text()));
	//	add1.text(String() + c.maximum());
	//});
	//Button minimum(frame, 0, 0, 0, 0, L"miniimum", [&] (Button::Event& ) {
	//	c.minimum(to<int>(inde.text()));
	//	add1.text(String() + c.minimum());
	//});
	//Button pageSize(frame, 0, 0, 0, 0, L"pageSize", [&] (Button::Event& ) {
	//	c.pageSize(to<int>(inde.text()));
	//	add1.text(String() + c.pageSize());
	//});
	//Button vertical(frame, 0, 0, 0, 0, L"vertical", [&] (Button::Event& ) {
	//	c.vertical(!c.vertical());
	//	c.resize();
	//	layout.perform();
	//});
	//Button thick(frame, 0, 0, 0, 0, L"thick", [&] (Button::Event& ) {
	//	c.thick(!c.thick());
	//	c.resize();
	//	layout.perform();
	//});
	//Edit height1(frame, 0, 0, 0, 0, 5);
	//height1.text(String() + c.size().height);
	//Button height(frame, 0, 0, 0, 0, L"height", [&] (Button::Event& ) {
	//	c.size(c.size().width, to<int>(height1.text()));
	//	height1.text(String() + c.size().height);
	//});
	//Button tickCount(frame, 0, 0, 0, 0, L"tickCount", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.tickCount());
	//});
	//Button tickStyle(frame, 0, 0, 0, 0, L"tickStyle", [&] (Button::Event& ) {
	//	switch ((int)c.tickStyle()) {
	//		case TrackBar::TickStyle::top    : c.tickStyle(TrackBar::TickStyle::bottom); break;
	//		case TrackBar::TickStyle::bottom : c.tickStyle(TrackBar::TickStyle::both); break;
	//		case TrackBar::TickStyle::both   : c.tickStyle(TrackBar::TickStyle::top); break;
	//	}
	//	c.resize();
	//	layout.perform();
	//});
	//Button tickVisible(frame, 0, 0, 0, 0, L"tickVisible", [&] (Button::Event& ) {
	//	c.tickVisible(!c.tickVisible());
	//	c.resize();
	//	layout.perform();
	//});
	//Edit tickFrequency1(frame, 0, 0, 0, 0, 5);
	//tickFrequency1.text(L"10");
	//Button tickFrequency(frame, 0, 0, 0, 0, L"tickFrequency", [&] (Button::Event& ) {
	//	c.tickFrequency(to<int>(tickFrequency1.text()));
	//});
	//Edit thumbSize1(frame, 0, 0, 0, 0, 5);
	//thumbSize1.text(String() + c.thumbSize());
	//Button thumbSize(frame, 0, 0, 0, 0, L"thumbSize", [&] (Button::Event& ) {
	//	c.thumbSize(to<int>(thumbSize1.text()));
	//	c.resize();
	//	layout.perform();
	//});
	//Button thumbVisible(frame, 0, 0, 0, 0, L"thumbVisible", [&] (Button::Event& ) {
	//	c.thumbVisible(!c.thumbVisible());
	//});
	//Button downIsLeft(frame, 0, 0, 0, 0, L"downIsLeft", [&] (Button::Event& ) {
	//	c.downIsLeft(!c.downIsLeft());
	//});
	//Button valueTip(frame, 0, 0, 0, 0, L"valueTip", [&] (Button::Event& ) {
	//	c = TrackBar(*c.parent(), 0, 0, 0, 0, c.minimum(), c.maximum(), TrackBar::Options(c.options() ^ TrackBar::Options::valueTip));
	//	c.zOrder(-1);
	//});
	//Button valueTipSide(frame, 0, 0, 0, 0, L"valueTipSide", [&] (Button::Event& ) {
	//	static bool side = true;
	//	c.valueTipSide(side ? TrackBar::ValueTipSide::bottom : TrackBar::ValueTipSide::top);
	//	side = !side;
	//});
	//Button clearTick(frame, 0, 0, 0, 0, L"clearTick", [&] (Button::Event& ) {
	//	c.clearTick();
	//});
	//Edit setTick1(frame, 0, 0, 0, 0, 5);
	//setTick1.text(L"10");
	//Button setTick(frame, 0, 0, 0, 0, L"setTick", [&] (Button::Event& ) {
	//	c.setTick(to<int>(setTick1.text()));
	//});
	//Button clearSelect(frame, 0, 0, 0, 0, L"clearSelect", [&] (Button::Event& ) {
	//	c.clearSelection();
	//});
	//layout.perform();




	//// ScrollBar 긡긚긣
	//ScrollBar cc(frame, 0, 0, 0, 0, 0, 100, 10);
	//ScrollBar c(move(cc));
	//cc = move(c);
	//c = move(cc);
	//c.lineSize(5);
	//c.brush(frameBrush);

	//Label add1(frame, 0, 0, 0, 0, L"0000");
	//c.onScroll() = [&] (ScrollBar::Scroll& e) {
	//	add1.text(String() + c.value());
	//};
	//add1.text(String() + c.value());
	//Edit add2(frame, 0, 0, 0, 0, 5);
	//add2.text(L"10");
	//Button value(frame, 0, 0, 0, 0, L"value", [&] (Button::Event& ) {
	//	c.value(to<int>(add2.text()));
	//	add1.text(String() + c.value());
	//});
	//Edit inde(frame, 0, 0, 0, 0, 5);
	//inde.text(L"100");
	//Button maximum(frame, 0, 0, 0, 0, L"maximum", [&] (Button::Event& ) {
	//	c.maximum(to<int>(inde.text()));
	//	add1.text(String() + c.maximum());
	//});
	//Button minimum(frame, 0, 0, 0, 0, L"miniimum", [&] (Button::Event& ) {
	//	c.minimum(to<int>(inde.text()));
	//	add1.text(String() + c.minimum());
	//});
	//Button pageSize(frame, 0, 0, 0, 0, L"pageSize", [&] (Button::Event& ) {
	//	c.pageSize(to<int>(inde.text()));
	//	add1.text(String() + c.pageSize());
	//});
	//Button vertical(frame, 0, 0, 0, 0, L"vertical", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	c = ScrollBar(*c.parent(), b.x, b.y, 0, 0, c.minimum(), c.maximum(), c.pageSize(), ScrollBar::Options(c.options() ^ ScrollBar::Options::vertical));
	//	c.zOrder(-1);
	//	layout.perform();
	//});




	//// ProgressBar 긡긚긣
	//ProgressBar cc(frame, 10, 10, 0, 0, 0, 100);
	//ProgressBar c(move(cc));
	//cc = move(c);
	//c = move(cc);

	//Label add1(frame, 0, 0, 0, 0, L"0000");
	//add1.text(String() + c.value());
	//Edit add2(frame, 0, 0, 0, 0, 5);
	//add2.text(L"10");
	//Button add(frame, 0, 0, 0, 0, L"add", [&] (Button::Event& ) {
	//	c.add(to<int>(add2.text()));
	//	add1.text(String() + c.value());
	//});
	//Edit inde(frame, 0, 0, 0, 0, 5);
	//inde.text(L"100");
	//Button intermediate(frame, 0, 0, 0, 0, L"intermediate", [&] (Button::Event& ) {
	//	if (c.indeterminate()) {
	//		c.indeterminate(false);
	//	} else {
	//		c.indeterminate(true, to<int>(inde.text()));
	//	}
	//});
	//Button maximum(frame, 0, 0, 0, 0, L"maximum", [&] (Button::Event& ) {
	//	c.maximum(to<int>(inde.text()));
	//	add1.text(String() + c.maximum());
	//});
	//Button minimum(frame, 0, 0, 0, 0, L"miniimum", [&] (Button::Event& ) {
	//	c.minimum(to<int>(inde.text()));
	//	add1.text(String() + c.minimum());
	//});
	//Button vertical(frame, 0, 0, 0, 0, L"vertical", [&] (Button::Event& ) {
	//	c.vertical(!c.vertical());
	//	c.resize();
	//	layout.perform();
	//});




	//// Monitor 긡긚긣
	//Debug::writeLine(String() + L"monitorsCount;" + Monitor::monitors().size());
	//Debug::writeLine(String() + L"monitorsHasSameBitsPerPixel;" + Monitor::monitorsHasSameBitsPerPixel());
	//Debug::writeLine(String() + L"virtualMonitorBounds;" + toStr(Monitor::virtualMonitorBounds()));
	//Debug::writeLine(String() + L"visibleMonitorsCount;" + Monitor::visibleMonitorsCount());
	//Debug::writeLine(String() + L"\n\nmaxsize:" + SystemInformation::primaryMonitorMaximizedWindowSize().width);
	//Debug::writeLine(String() + L"\n\nmaxsize:" + SystemInformation::primaryMonitorMaximizedWindowSize().height);

	//auto test = [&] (Monitor&& m) {
	//	Debug::writeLine(String() + L"name;" + m.deviceName());
	//	Debug::writeLine(String() + L"bits;" + m.bitsPerPixel());
	//	Debug::writeLine(String() + L"bounds;" + toStr(m.bounds()));
	//	Debug::writeLine(String() + L"orientation;" + (int)m.orientation());
	//	Debug::writeLine(String() + L"primary;" + m.primary());
	//	Debug::writeLine(String() + L"refreshRage;" + m.refreshRate());
	//	Debug::writeLine(String() + L"workingArea;" + toStr(m.workingArea()));
	//};

	//Debug::writeLine(L"\n\ndefault:\n");
	//test(Monitor());
	//Debug::writeLine(L"\n\nprimary:\n");
	//test(Monitor::primaryMonitor());



	//// DragDrop 긡긚긣
	//Bitmap bm;
	//CheckBox blend(frame, 0, 0, 0, 0, L"blend");
	//CheckBox dib(frame, 0, 0, 0, 0, L"dib");
	//frame.onPaint() = [&] (Frame::Paint& e) {
	//	e.graphics().brush(Color::control());
	//	e.graphics().clear();
	//	if ((Bitmap::HBITMAP)bm) {
	//		e.graphics().blend(0, frame.getPreferredSize().height, Graphics(bm), 1.0, blend.checked());
	//	}
	//};
	//Edit setBitmap1(frame, 0, 0, 0, 0, 50);
	//setBitmap1.text(L"C:\\Users\\syanji\\Desktop\\뗢긚?귽깑0.bmp");
	//DragDrop::Source source(frame);
	//DragDrop::Target target(frame);
	//frame.onMouseMove() = [&] (Frame::MouseMove& e) {
	//	if (e.lButton()) {
	//		Bitmap bitmap;
	//		if (setBitmap1.text().empty()) {
	//			bitmap = Bitmap::createDDB(400, 300);
	//			Graphics g(bitmap);
	//			g.brush(Color::blue());
	//			g.clear();
	//			g.brush(Color::yellow());
	//			g.drawEllipse(0, 0, bitmap.width(), bitmap.height());
	//		} else {
	//			bitmap = Bitmap(setBitmap1.text());
	//			Graphics g(bitmap);
	//			g.brush(Brush::hollow());
	//			g.pen(Color::red());
	//			g.drawRectangle(0, 0, bitmap.width(), bitmap.height());
	//		}
	//		DragDrop::Data data;
	//		if (dib.checked()) {
	//			data.setDIB(bitmap);
	//		} else {
	//			data.setBitmap(bitmap);
	//		}
	//		source.doDragDrop(data);
	//	}
	//};
	//target.onDrop() = [&] (DragDrop::Drop& e) {
	//	if (e.data().containsBitmap()) {
	//		bm = e.data().getBitmap();
	//		if (blend.checked()) {
	//			bm.premultiplyAlpha();
	//		}
	//		frame.invalidate();
	//		MsgBox::show(L"bitmap");
	//	} else if (e.data().containsDIB()) {
	//		bm = e.data().getDIB();
	//		if (blend.checked()) {
	//			bm.premultiplyAlpha();
	//		}
	//		frame.invalidate();
	//		MsgBox::show(L"dib");
	//	} else if (e.data().containsFileDropList()) {
	//		auto list = e.data().getFileDropList();
	//		String s;
	//		for (auto i = list.begin(); i != list.end(); ++i) {
	//			s += L":" + *i + L":\n";
	//		}
	//		Debug::write(s);
	//		MsgBox::show(s);
	//	} else if (e.data().containsText()) {
	//		MsgBox::show(e.data().getText());
	//	}
	//};
	////Button setFileDropList(frame, 0, 0, 0, 0, L"setFileDropList", [&] (Button::Event& ) {
	////	auto list = File(setBitmap1.text()).getFiles();
	////	vector<String> files;
	////	for (auto i = list.begin(), end = list.end(); i != end; ++i) {
	////		files.push_back(i->path());
	////	}
	////	Clipboard::setFileDropList(files);
	////});
	//layout.perform();




	//// Clipboard 긡긚긣
	//Bitmap bm;
	//Button clear(frame, 0, 0, 0, 0, L"clear", [&] (Button::Event& ) {
	//	Clipboard::clear();
	//});
	//CheckBox blend(frame, 0, 0, 0, 0, L"blend");
	//frame.onPaint() = [&] (Frame::Paint& e) {
	//	e.graphics().brush(Color::control());
	//	e.graphics().clear();
	//	if ((Bitmap::HBITMAP)bm) {
	//		e.graphics().blend(0, frame.getPreferredSize().height, Graphics(bm), 1.0, blend.checked());
	//	}
	//};
	//Button containsBitmap(frame, 0, 0, 0, 0, L"containsBitmap", [&] (Button::Event& ) {
	//	if (Clipboard::containsBitmap()) {
	//		bm = Clipboard::getBitmap();
	//		if ((Bitmap::HBITMAP)bm) {
	//			if (blend.checked()) {
	//				bm.premultiplyAlpha();
	//			}
	//			frame.invalidate();
	//		} else {
	//			MsgBox::show(L"롦벦렪봲");
	//		}
	//	}
	//	MsgBox::show(String() + Clipboard::containsBitmap());
	//});
	//Button containsDIB(frame, 0, 0, 0, 0, L"containsDIB", [&] (Button::Event& ) {
	//	if (Clipboard::containsDIB()) {
	//		bm = Clipboard::getDIB();
	//		if ((Bitmap::HBITMAP)bm) {
	//			if (blend.checked()) {
	//				bm.premultiplyAlpha();
	//			}
	//			frame.invalidate();
	//		} else {
	//			MsgBox::show(L"롦벦렪봲");
	//		}
	//	}
	//	MsgBox::show(String() + Clipboard::containsDIB());
	//});
	//Button containsFileDropList(frame, 0, 0, 0, 0, L"containsFileDropList", [&] (Button::Event& ) {
	//	if (Clipboard::containsFileDropList()) {
	//		auto list = Clipboard::getFileDropList();
	//		String s;
	//		for (auto i = list.begin(); i != list.end(); ++i) {
	//			s += L":" + *i + L":\n";
	//		}
	//		Debug::write(s);
	//		MsgBox::show(s);
	//	} else {
	//		MsgBox::show(String() + Clipboard::containsFileDropList());
	//	}
	//});
	//Edit containsMemory1(frame, 0, 0, 0, 0, 10);
	//Button containsMemory(frame, 0, 0, 0, 0, L"containsMemory", [&] (Button::Event& ) {
	//	int format = 0;
	//	if (!containsMemory1.text().empty()) {
	//		format = Convert::to<int>(containsMemory1.text());
	//	}
	//	if (Clipboard::containsMemory(format)) {
	//		MemoryStream stream = Clipboard::getMemory(format);
	//		if (stream.length()) {
	//			String s(L'a', (int)stream.length() / 2 - 1);
	//			stream.read((void*)s.c_str(), 0, s.length() * 2);
	//			MsgBox::show(L":" + s + L":");
	//		} else {
	//			MsgBox::show(L"empty");
	//		}
	//	} else {
	//		MsgBox::show(L"false");
	//	}
	//});
	//Button containsText(frame, 0, 0, 0, 0, L"containsText", [&] (Button::Event& ) {
	//	if (Clipboard::containsText()) {
	//		MsgBox::show(L"걏" + Clipboard::getText() + L"걐");
	//	} else {
	//		MsgBox::show(String() + Clipboard::containsText());
	//	}
	//});
	//Button registerMemoryFormat(frame, 0, 0, 0, 0, L"registerMemoryFormat", [&] (Button::Event& ) {
	//	MsgBox::show(String() + Clipboard::registerMemoryFormat(L"memoryTest"));
	//});
	//Edit setBitmap1(frame, 0, 0, 0, 0, 50);
	//setBitmap1.text(L"C:\\Users\\syanji\\Desktop\\뗢긚?귽깑0.bmp");
	//Button setBitmap(frame, 0, 0, 0, 0, L"setBitmap", [&] (Button::Event& ) {
	//	Bitmap bitmap;
	//	if (setBitmap1.text().empty()) {
	//		bitmap = Bitmap::createDDB(400, 300);
	//		Graphics g(bitmap);
	//		g.brush(Color::blue());
	//		g.clear();
	//		g.brush(Color::yellow());
	//		g.drawEllipse(0, 0, bitmap.width(), bitmap.height());
	//	} else {
	//		bitmap = Bitmap(setBitmap1.text());
	//		Graphics g(bitmap);
	//		g.brush(Brush::hollow());
	//		g.pen(Color::red());
	//		g.drawRectangle(0, 0, bitmap.width(), bitmap.height());
	//	}
	//	Clipboard::setBitmap(bitmap);
	//});
	//Button setDIB(frame, 0, 0, 0, 0, L"setDIB", [&] (Button::Event& ) {
	//	Bitmap bitmap;
	//	if (setBitmap1.text().empty()) {
	//		bitmap = Bitmap::createDDB(400, 300);
	//		Graphics g(bitmap);
	//		g.brush(Color::blue());
	//		g.clear();
	//		g.brush(Color::yellow());
	//		g.drawEllipse(0, 0, bitmap.width(), bitmap.height());
	//	} else {
	//		bitmap = Bitmap(setBitmap1.text());
	//		Graphics g(bitmap);
	//		g.brush(Brush::hollow());
	//		g.pen(Color::red());
	//		g.drawRectangle(0, 0, bitmap.width(), bitmap.height());
	//	}
	//	Clipboard::setDIB(bitmap);
	//});
	//Button setFileDropList(frame, 0, 0, 0, 0, L"setFileDropList", [&] (Button::Event& ) {
	//	auto list = File(setBitmap1.text()).getFiles();
	//	vector<String> files;
	//	for (auto i = list.begin(), end = list.end(); i != end; ++i) {
	//		files.push_back(i->path());
	//	}
	//	Clipboard::setFileDropList(files);
	//});
	//Button setMemory(frame, 0, 0, 0, 0, L"setMemory", [&] (Button::Event& ) {
	//	MemoryStream stream;
	//	auto s = setBitmap1.text();
	//	stream.write(s.c_str(), 0, s.length() * 2 + 2);
	//	stream.position(0);
	//	Clipboard::setMemory(Clipboard::registerMemoryFormat(L"memoryTest"), stream);
	//});
	//Button setText(frame, 0, 0, 0, 0, L"setText", [&] (Button::Event& ) {
	//	Clipboard::setText(setBitmap1.text());
	//});
	//layout.perform();




	//// Edit 긡긚긣
	//Edit cc(frame, 0, 0, 0, 0, 10, 1);// 3, Edit::Options::multiline | Edit::Options::noHScroll);
	//Edit c(move(cc));
	//cc = move(c);
	//c = move(cc);
	////Font font30(c.font(), 30, Font::Style::regular);
	////c.font(font30);
	////c.edge(Edit::Edge::none);
	////c.preferredCharSize(30, 3);
	////c.hScrollBar(true);
	////c.vScrollBar(true);
	////c.margin(0, 0, 0, 0);
	////c.margin(10, 15, 10, 15);
	////c.margin(10, 0, 20, 0);
	////c.resize();
	////c.onShortcutKey() = [&] (Edit::ShortcutKey& e) {
	////	switch (e.shortcut()) {
	////		case Key::tab : e.isInputKey(true); break;
	////		case Key::escape : e.isInputKey(true); break;
	////		case Key::enter : e.isInputKey(true); break;
	////	}
	////};

	//std::wregex cRegex(L"-?[0-9]*\\.?[0-9]*");
	////std::wregex cRegex(L"[0-9a-zA-Z궇-귪]*");
	//c.onTextChange() = [&] (Edit::TextChange& e) {
	//	//StringBuffer buffer(e.newText());
	//	//for (auto i = buffer.begin(), end = buffer.end(); i != end; ++i) {
	//	//	if (L'궆' <= *i && *i <= L'귪') {
	//	//		*i += L'?' - L'궆';
	//	//	}
	//	//}
	//	//e.newText(buffer);

	//	if (!std::regex_match(e.newText().c_str(), cRegex)) {
	//		e.cancel(true);
	//	}
	//};
	////c.onKeyPress() = [&] (Edit::KeyPress& e) {
	////	if (L'궆' <= e.charCode() && e.charCode() <= L'귪') {
	////		e.charCode(e.charCode() + L'?' - L'궆');
	////	}
	////};
	//c.onPaste() = [&] (Edit::Paste& e) {
	//	Debug::writeLine(L"onPaste");
	//	//e.cancel(true);
	//};
	//c.onMouseDown() = [&] (Edit::MouseDown& e) {
	//	if (e.button() == Mouse::mButton) {
	//		Debug::writeLine(String() + c.getIndexAt(c.screenToClient(Mouse::position())));
	//	}
	//};

	//String items[] = {L"귺귽긂", L"귺귺긂", L"012", L"abc", L"abd", L"abcdef"};
	//AutoComplete acc(c, items);
	////AutoComplete acc(c, AutoComplete::SystemItems::fileSystem);


	//Button edgeButton(frame,0, 0, 0, 0, L"edge", [&] (Button::Event& ) {
	//	int edge = c.edge();
	//	++edge;
	//	if (3 < edge) {
	//		edge = 0;
	//	}
	//	c.edge(Control::Edge(edge));
	//	c.resize();
	//});
	//Button textAlignButton(frame, 0, 0, 0, 0, L"align", [&] (Button::Event& ) {
	//	int align = c.align();
	//	++align;
	//	if (2 < align) {
	//		align = 0;
	//	}
	//	c.align(Edit::Align(align));
	//});
	//Button hScrollBar(frame, 0, 0, 0, 0, L"hScrollBar", [&] (Button::Event& ) {
	//	c.hScrollBar(!c.hScrollBar());
	//	c.resize();
	//});
	//Button vScrollBar(frame, 0, 0, 0, 0, L"vScrollBar", [&] (Button::Event& ) {
	//	c.vScrollBar(!c.vScrollBar());
	//	c.resize();
	//});
	//Button lowercaseOnly(frame, 0, 0, 0, 0, L"lowercaseOnly", [&] (Button::Event& ) {
	//	c.lowercaseOnly(!c.lowercaseOnly());
	//});
	//Button uppercaseOnly(frame, 0, 0, 0, 0, L"uppercaseOnly", [&] (Button::Event& ) {
	//	c.uppercaseOnly(!c.uppercaseOnly());
	//});
	//Button numberOnly(frame, 0, 0, 0, 0, L"numberOnly", [&] (Button::Event& ) {
	//	c.numberOnly(!c.numberOnly());
	//});
	//Button passwordChar(frame, 0, 0, 0, 0, L"passwordChar", [&] (Button::Event& ) {
	//	c.passwordChar(!c.passwordChar() ? L'*' : L'\0');
	//});
	//Button canUndo(frame, 0, 0, 0, 0, L"canUndo", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.canUndo());
	//});
	//Button clearUndo(frame, 0, 0, 0, 0, L"clearUndo", [&] (Button::Event& ) {
	//	c.clearUndo();
	//});
	//Button copy(frame, 0, 0, 0, 0, L"copy", [&] (Button::Event& ) {
	//	c.copy();
	//});
	//Button cut(frame, 0, 0, 0, 0, L"cut", [&] (Button::Event& ) {
	//	c.cut();
	//});
	//Button paste(frame, 0, 0, 0, 0, L"paste", [&] (Button::Event& ) {
	//	c.paste();
	//});
	//Button undo(frame, 0, 0, 0, 0, L"undo", [&] (Button::Event& ) {
	//	c.undo();
	//});
	//Button multiline(frame, 0, 0, 0, 0, L"multiline", [&] (Button::Event& ) {
	//	Debug::writeLine(String() + L"multi:" + c.selection().index + L":" + c.selection().length);
	//});
	//Button readOnly(frame, 0, 0, 0, 0, L"readOnly", [&] (Button::Event& ) {
	//	c.readOnly(!c.readOnly());
	//});
	//Button maxLength(frame, 0, 0, 0, 0, L"maxLength", [&] (Button::Event& ) {
	//	c.maxLength(c.maxLength() != 3 ? 3 : 10);
	//});
	//Button noHScroll(frame, 0, 0, 0, 0, L"noHScroll", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	c = Edit(*c.parent(), b.x, b.y, b.width, b.height, Edit::Options(c.options() ^ Edit::Options::noHScroll));
	//	c.zOrder(-1);
	//});
	//Button noVScroll(frame, 0, 0, 0, 0, L"noVScroll", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	c = Edit(*c.parent(), b.x, b.y, b.width, b.height, Edit::Options(c.options() ^ Edit::Options::noVScroll));
	//	c.zOrder(-1);
	//});
	//Button alwaysSelected(frame, 0, 0, 0, 0, L"alwaysSelected", [&] (Button::Event& ) {
	//	auto b = c.bounds();
	//	c = Edit(*c.parent(), b.x, b.y, b.width, b.height, Edit::Options(c.options() ^ Edit::Options::alwaysSelected));
	//	c.zOrder(-1);
	//});
	//Button firstVisibleLine(frame, 0, 0, 0, 0, L"firstVisibleLine", [&] (Button::Event& ) {
	//	Debug::writeLine(String() + c.firstVisibleLine());
	//});
	//Button cueBanner(frame, 0, 0, 0, 0, L"cueBanner", [&] (Button::Event& ) {
	//	c.cueBanner(L"룊딖?렑");
	//	assert(c.cueBanner() == L"룊딖?렑");
	//});
	//Button noKeyPress(frame, 0, 0, 0, 0, L"noKeyPress", [&] (Button::Event& ) {
	//	auto s = toFlag(c.imeStatus());
	//	s.set(Edit::ImeStatus::noKeyPress, !s[Edit::ImeStatus::noKeyPress]);
	//	c.imeStatus(s);
	//});
	//Button cancelOnGotFocus(frame, 0, 0, 0, 0, L"cancelOnGotFocus", [&] (Button::Event& ) {
	//	auto s = toFlag(c.imeStatus());
	//	s.set(Edit::ImeStatus::cancelOnGotFocus, !s[Edit::ImeStatus::cancelOnGotFocus]);
	//	c.imeStatus(s);
	//});
	//Button completeOnLostFocus(frame, 0, 0, 0, 0, L"completeOnLostFocus", [&] (Button::Event& ) {
	//	auto s = toFlag(c.imeStatus());
	//	s.set(Edit::ImeStatus::completeOnLostFocus, !s[Edit::ImeStatus::completeOnLostFocus]);
	//	c.imeStatus(s);
	//});
	//Button lineCount(frame, 0, 0, 0, 0, L"lineCount", [&] (Button::Event& ) {
	//	MsgBox::show(String() + c.lineCount());
	//});
	//Button caretIndex(frame, 0, 0, 0, 0, L"carentIndex", [&] (Button::Event& ) {
	//	//c.focus();
	//	MsgBox::show(String() + c.caretIndex());
	//});
	//Edit caretSet(frame, 0, 0, 0, 0, 1);
	//Button caretOk(frame, 0, 0, 0, 0, L"ok", [&] (Button::Event& ) {
	//	auto text = caretSet.text();
	//	if (!text.empty()) {
	//		int i = Convert::to<int>(text);
	//		c.caretIndex(i);
	//	}
	//});
	//Button getIndexFromLine(frame, 0, 0, 0, 0, L"getIndexFromLine", [&] (Button::Event& ) {
	//	for (int i = 0; i < c.lineCount(); ++i) {
	//		Debug::writeLine(String() + L"line:" + i + L" = " + c.getIndexFromLine(i));
	//	}
	//});
	//Button getLineFromIndex(frame, 0, 0, 0, 0, L"getLineFromIndex", [&] (Button::Event& ) {
	//	for (int i = 0; i <= c.length(); ++i) {
	//		Debug::writeLine(String() + L"index:" + i + L" = " + c.getLineFromIndex(i));
	//	}
	//});
	//Button getLineLength(frame, 0, 0, 0, 0, L"getLineLength", [&] (Button::Event& ) {
	//	for (int i = 0; i < c.lineCount(); ++i) {
	//		Debug::writeLine(String() + L"line:" + i + L" = " + c.getLineLength(i));
	//	}
	//});
	//Button getLineText(frame, 0, 0, 0, 0, L"getLineText", [&] (Button::Event& ) {
	//	for (int i = 0; i < c.lineCount(); ++i) {
	//		Debug::writeLine(String() + L"line:" + i + L" = " + c.getLineText(i));
	//	}
	//});
	//Button scrollToCaret(frame, 0, 0, 0, 0, L"scrollToCaret", [&] (Button::Event& ) {
	//	c.scrollToCaret();
	//});
	//Button scrollLineDown(frame, 0, 0, 0, 0, L"scrollLineDown", [&] (Button::Event& ) {
	//	c.scrollLineDown();
	//});
	//Button scrollLineUp(frame, 0, 0, 0, 0, L"scrollLineUp", [&] (Button::Event& ) {
	//	c.scrollLineUp();
	//});
	//Button scrollPageDown(frame, 0, 0, 0, 0, L"scrollPageDown", [&] (Button::Event& ) {
	//	c.scrollPageDown();
	//});
	//Button scrollPageUp(frame, 0, 0, 0, 0, L"scrollPageUp", [&] (Button::Event& ) {
	//	c.scrollPageUp();
	//});
	//Edit scroll1(frame, 0, 0, 0, 0, 2);
	//Edit scroll2(frame, 0, 0, 0, 0, 2);
	//Button scroll(frame, 0, 0, 0, 0, L"scroll", [&] (Button::Event& ) {
	//	int x = 0, y = 0;
	//	if (scroll1.length()) {
	//		x = Convert::to<int>(scroll1.text());
	//	}
	//	if (scroll2.length()) {
	//		y = Convert::to<int>(scroll2.text());
	//	}
	//	c.scrollInto(x, y);
	//});
	//Edit tabWidth1(frame, 0, 0, 0, 0, 2);
	//Button tabWidth(frame, 0, 0, 0, 0, L"tabWidth", [&] (Button::Event& ) {
	//	int i = 0;
	//	if (tabWidth1.length()) {
	//		i = Convert::to<int>(tabWidth1.text());
	//	}
	//	//c.tabWidth(i);
	//	MsgBox::show(String() + tabWidth1.caretIndex());
	//});
	//Button caretVisible(frame, 0, 0, 0, 0, L"caretVisible", [&] (Button::Event& ) {
	//	c.caretVisible(!c.caretVisible());
	//});
	//bool change = false;
	//Bitmap car(6, 12);
	//{
	//	Graphics g(car);
	//	//g.brush(Color::black());
	//	//g.clear();
	//	g.brush(Color::white());
	//	g.drawRectangle(0, 0, 6, 12);
	//}
	//Button changeCaret(frame, 0, 0, 0, 0, L"changeCaret", [&] (Button::Event& ) {
	//	if (change) {
	//		c.changeCaret(1, c.font().height());
	//	} else {
	//		Graphics g(c);
	//		g.font(c.font());
	//		c.changeCaret(g.measureText(L"0"), true);
	//	}
	//	change = !change;
	//});
	//Button changeCaret2(frame, 0, 0, 0, 0, L"changeCaret2", [&] (Button::Event& ) {
	//	if (change) {
	//		c.changeCaret(1, c.font().height());
	//	} else {
	//		c.changeCaret(car);
	//	}
	//	change = !change;
	//});

	//c.onMouseDown() = [&] (Control::MouseDown& e) {
	//	//c.caretVisible(!c.caretVisible());
	//	//if (change) {
	//	//	c.changeCaret(1, c.font().height());
	//	//} else {
	//	//	c.changeCaret(car);
	//	//}
	//	change = !change;
	//};




	////Label 긡긚긣
	//int width = 0;
	//Bitmap bitmap2(200, 200);
	//{
	//	Graphics g(bitmap2);
	//	g.brush(Color::red());
	//	g.clear();
	//	g.brush(Color::white());
	//	g.drawEllipse(0, 0, 200, 200);
	//}
	//Label cc(frame, 10, 20, width, 0, L"&cont&&roloijo\\lljlllllkkkkkkk\\hoge\\momomom");
	////Label cc(frame, 10, 20, 300, 300, bitmap2);
	//Label c(move(cc));
	//cc = move(c);
	//c = move(cc);
	////c.edge(Label::Edge::line);
	//int edge = (int)c.edge();
	//layout.perform();
	//c.textColor(Color::red());
	//c.brush(frameBrush);
	//c.brushOrigin(-c.position() - c.clientOrigin());
	//Button edgeButton(frame, frame.lastBounds().x, frame.lastBounds().bottom() + 50, 0, 0, L"edge", [&] (Button::Event& ) {
	//	++edge;
	//	if (3 < edge) {
	//		edge = 0;
	//	}
	//	c.edge(Control::Edge(edge));
	//	//c.size(c.getPreferredSize(width, 0));
	//	//assert(c.clientSize() == bitmap.size());
	//});
	//int textAlign = (int)c.textAlign();
	//Button textAlignButton(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"textAlign", [&] (Button::Event& ) {
	//	++textAlign;
	//	if (2 < textAlign) {
	//		textAlign = 0;
	//	}
	//	c.textAlign(Label::TextAlign(textAlign));
	//	c.size(c.getPreferredSize(width, 0));
	//});
	//Button endEllipsis(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"endEllipsis", [&] (Button::Event& ) {
	//	c.endEllipsis(!c.endEllipsis());
	//	c.size(c.getPreferredSize(width, 0));
	//});
	//Button noPrefix(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"noPrefix", [&] (Button::Event& ) {
	//	c.noPrefix(!c.noPrefix());
	//	c.size(c.getPreferredSize(width, 0));
	//});
	//Button pathEllipsis(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"pathEllipsis", [&] (Button::Event& ) {
	//	c.pathEllipsis(!c.pathEllipsis());
	//	c.size(c.getPreferredSize(width, 0));
	//});
	//Button centerImage(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"centerImage", [&] (Button::Event& ) {
	//	c.centerImage(!c.centerImage());
	//	//c.size(c.getPreferredSize(width, 0));
	//});
	//Button image(frame, frame.lastBounds().right() + 20, frame.lastBounds().y, 0, 0, L"image", [&] (Button::Event& ) {
	//	c.bitmap(c.bitmap() ? nullptr : (Bitmap::HBITMAP)bitmap2);
	//	c.size(c.getPreferredSize(width, 0));
	//});

	//DockLayout l0(frame);
	//l0.style(c, DockLayout::Style::fill);

	//frame.onResized() = [&] (Frame::Resized& e) {
	//	l0.perform();
	//};




	//// Panel 긡긚긣
	//Panel cc(frame, 0, 0, 150, 150, Control::Edge::client);
	//Panel c(move(cc));
	//cc = move(c);
	//c = move(cc);
	//c.text(L"control");
	//c.onPaint() = [&] (Panel::Paint& e) {
	//	e.graphics().brush(Color::green());
	//	e.graphics().clear();
	//};
	//Button button(c, 60, 80, 0, 0, L"button", [&] (Button::Event& ) {
	//	MsgBox::show(L"pushed");
	//});
	//Button edge(frame, 0, 0, 0, 0, L"edge", [&] (Button::Event& ) {
	//	int edge = c.edge();
	//	++edge;
	//	if (3 < edge) {
	//		edge = 0;
	//	}
	//	c.edge(Control::Edge(edge));
	//});
	//Button transparent(frame, 0, 0, 0, 0, L"transparent", [&] (Button::Event& ) {
	//	button.parent(&frame);
	//	c = Panel(*c.parent(), 0, 0, 150, 150, c.edge(), Panel::Options(c.options() ^ Panel::Options::transparent));
	//	c.zOrder(-1);
	//	button.parent(&c);
	//	layout.perform();
	//});
	////DockLayout l0(frame);
	////l0.style(c, DockLayout::Style::fill);

	////frame.onResized() = [&] (Frame::Resized& e) {
	////	l0.perform();
	////};




	//// GroupBox 긡긚긣
	//GroupBox cc(frame, 10, 40, 150, 60, L"control");
	//GroupBox c(move(cc));
	//cc = move(c);
	//c = move(cc);

	////c.onPrePaint() = [&] (GroupBox::PrePaint& e) {
	////	e.brush(brush);
	////	e.brushOrigin(-c.position());
	////};

	//RadioButton rb(c, 10, 80, 300, 0, L"cont\nrol");
	////rb.multiline(true);
	////rb.size(rb.getPreferredSize());
	////rb.onPaint() = [&] (Button::Paint& e) {
	////	auto g = e.graphics();
	////	if (e.sender().enabled()) {
	////		if (e.sender().pushed()) {
	////			g.brush(Color::blue());
	////		} else if (e.sender().hot()) {
	////			g.brush(Color::cyan());
	////		} else {
	////			g.brush(brush);
	////		}
	////	} else {
	////		g.brush(Color::gray());
	////	}
	////	auto r = e.sender().bounds();
	////	r.position(0, 0);
	////	g.drawRectangle(r);
	////	if (e.sender().focused()) {
	////		g.brush(Brush::hollow());
	////		r.grow(-5, -5);
	////		g.drawRectangle(r);
	////	}
	////};
	//c.resize();

	//DockLayout l0(frame);
	//l0.style(c, DockLayout::Style::fill);

	//DockLayout l1(c);
	//l1.style(rb, DockLayout::Style::right);

	//frame.onResized() = [&] (Frame::Resized& e) {
	//	l0.perform();
	//	l1.perform();
	//};




	//// RadioButton 긡긚긣
	//RadioButton cc(frame, 0, 0, 0, 0, L"control\n2line");
	//RadioButton c(move(cc));
	//cc = move(c);
	//c = move(cc);

	//RadioButton rb1(frame, 0, 0, 0, 0, L"control1");
	//RadioButton rb2(frame, 0, 0, 0, 0, L"control2");
	//RadioButton rb3(frame, 0, 0, 0, 0, L"control3");

	//Panel p0(frame, 0, 0, 200, 200, Control::Edge::client);

	//RadioButton rb4(p0, 10, 20, 0, 0, L"control4");
	//RadioButton rb5(p0, p0.lastBounds().x, p0.lastBounds().bottom() + 10, 0, 0, L"control5");
	//RadioButton rb6(p0, p0.lastBounds().x, p0.lastBounds().bottom() + 10, 0, 0, L"control6");

	//Panel p1(frame, 0, 0, 200, 200, Control::Edge::client);
	//RadioButton rb7(p1, 10, 20, 0, 0, L"control7");
	//RadioButton rb8(p1, p1.lastBounds().x, p1.lastBounds().bottom() + 10, 0, 0, L"control8");
	//RadioButton rb9(p1, p1.lastBounds().x, p1.lastBounds().bottom() + 10, 0, 0, L"control9");

	//c.startGroup(true);
	//rb2.startGroup(true);
	//rb4.startGroup(true);
	//rb7.startGroup(true);

	//Button multiline(frame, 0, 0, 0, 0, L"multiline", [&] (Button::Click& ) {
	//	c.multiline(!c.multiline());
	//});
	//Edit text0(frame, 0, 0, 0, 0, 20);
	//text0.text(c.text());
	//Button text(frame, 0, 0, 0, 0, L"text", [&] (Button::Click& ) {
	//	c.text(text0.text());
	//	c.resize();
	//	layout.perform();
	//});
	//auto onPaint0 = [&] (RadioButton::Paint& e) {
	//	auto& g = e.graphics();
	//	if (e.sender().enabled()) {
	//		if (e.sender().pushed()) {
	//			g.brush(Color::blue());
	//		} else if (e.sender().hot()) {
	//			g.brush(Color::cyan());
	//		} else {
	//			g.brush(Color::green());
	//		}
	//	} else {
	//		g.brush(Color::gray());
	//	}
	//	auto r = e.sender().bounds();
	//	r.position(0, 0);
	//	g.drawRectangle(r);
	//	if (e.sender().checked()) {
	//		g.brush(Color::red());
	//		g.drawRectangle(2, 2, 30, 30);
	//	}
	//	if (e.sender().focused()) {
	//		g.brush(Brush::hollow());
	//		r.grow(-5, -5);
	//		g.drawRectangle(r);
	//	}
	//};
	//Button onPaint(frame, 0, 0, 0, 0, L"onPaint", [&] (Button::Click& ) {
	//	if (c.onPaint()) {
	//		c.onPaint().pop();
	//	} else {
	//		c.onPaint() = onPaint0;
	//	}
	//	c.invalidate();
	//});
	//Button textAlign(frame, 0, 0, 0, 0, L"testAlign", [&] (Button::Event& ) {
	//	switch ((int)c.textAlign()) {
	//		case RadioButton::TextAlign::topLeft      : c.textAlign(RadioButton::TextAlign::topCenter   ); break;
	//		case RadioButton::TextAlign::topCenter    : c.textAlign(RadioButton::TextAlign::topRight    ); break;
	//		case RadioButton::TextAlign::topRight     : c.textAlign(RadioButton::TextAlign::middleLeft  ); break;
	//		case RadioButton::TextAlign::middleLeft   : c.textAlign(RadioButton::TextAlign::middleCenter); break;
	//		case RadioButton::TextAlign::middleCenter : c.textAlign(RadioButton::TextAlign::middleRight ); break;
	//		case RadioButton::TextAlign::middleRight  : c.textAlign(RadioButton::TextAlign::bottomLeft  ); break;
	//		case RadioButton::TextAlign::bottomLeft   : c.textAlign(RadioButton::TextAlign::bottomCenter); break;
	//		case RadioButton::TextAlign::bottomCenter : c.textAlign(RadioButton::TextAlign::bottomRight ); break;
	//		case RadioButton::TextAlign::bottomRight  : c.textAlign(RadioButton::TextAlign::topLeft     ); break;
	//	}
	//});
	//Button autoCheck(frame, 0, 0, 0, 0, L"autoCheck", [&] (Button::Event& ) {
	//	c.autoCheck(!c.autoCheck());
	//});
	//Button pushLike(frame, 0, 0, 0, 0, L"pushLike", [&] (Button::Event& ) {
	//	c.pushLike(!c.pushLike());
	//	c.resize();
	//});
	//Button rightButton(frame, 0, 0, 0, 0, L"rightButton", [&] (Button::Event& ) {
	//	c.rightButton(!c.rightButton());
	//});
	//Button checked(frame, 0, 0, 0, 0, L"checked", [&] (Button::Event& ) {
	//	c.checked(!c.checked());
	//});
	//c.onCheck() = [&] (RadioButton::Event& ) {
	//	Debug::writeLine(L"onChecked");
	//	//c.checked(!c.checked());
	//};




	//// CheckBox 긡긚긣
	//CheckBox cc(frame, 0, 0, 0, 0, L"control\n2line");
	//CheckBox c(move(cc));
	//cc = move(c);
	//c = move(cc);

	//Button multiline(frame, 0, 0, 0, 0, L"multiline", [&] (Button::Click& ) {
	//	c.multiline(!c.multiline());
	//});
	//Edit text0(frame, 0, 0, 0, 0, 20);
	//text0.text(c.text());
	//Button text(frame, 0, 0, 0, 0, L"text", [&] (Button::Click& ) {
	//	c.text(text0.text());
	//	c.resize();
	//	layout.perform();
	//});
	//auto onPaint0 = [&] (CheckBox::Paint& e) {
	//	auto& g = e.graphics();
	//	if (e.sender().enabled()) {
	//		if (e.sender().pushed()) {
	//			g.brush(Color::blue());
	//		} else if (e.sender().hot()) {
	//			g.brush(Color::cyan());
	//		} else {
	//			g.brush(Color::green());
	//		}
	//	} else {
	//		g.brush(Color::gray());
	//	}
	//	auto r = e.sender().bounds();
	//	r.position(0, 0);
	//	g.drawRectangle(r);
	//	if (e.sender().checked()) {
	//		g.brush(Color::red());
	//		g.drawRectangle(2, 2, 30, 30);
	//	}
	//	if (e.sender().focused()) {
	//		g.brush(Brush::hollow());
	//		r.grow(-5, -5);
	//		g.drawRectangle(r);
	//	}
	//};
	//Button onPaint(frame, 0, 0, 0, 0, L"onPaint", [&] (Button::Click& ) {
	//	if (c.onPaint()) {
	//		c.onPaint().pop();
	//	} else {
	//		c.onPaint() = onPaint0;
	//	}
	//	c.invalidate();
	//});
	//Button textAlign(frame, 0, 0, 0, 0, L"testAlign", [&] (Button::Event& ) {
	//	switch ((int)c.textAlign()) {
	//		case CheckBox::TextAlign::topLeft      : c.textAlign(CheckBox::TextAlign::topCenter   ); break;
	//		case CheckBox::TextAlign::topCenter    : c.textAlign(CheckBox::TextAlign::topRight    ); break;
	//		case CheckBox::TextAlign::topRight     : c.textAlign(CheckBox::TextAlign::middleLeft  ); break;
	//		case CheckBox::TextAlign::middleLeft   : c.textAlign(CheckBox::TextAlign::middleCenter); break;
	//		case CheckBox::TextAlign::middleCenter : c.textAlign(CheckBox::TextAlign::middleRight ); break;
	//		case CheckBox::TextAlign::middleRight  : c.textAlign(CheckBox::TextAlign::bottomLeft  ); break;
	//		case CheckBox::TextAlign::bottomLeft   : c.textAlign(CheckBox::TextAlign::bottomCenter); break;
	//		case CheckBox::TextAlign::bottomCenter : c.textAlign(CheckBox::TextAlign::bottomRight ); break;
	//		case CheckBox::TextAlign::bottomRight  : c.textAlign(CheckBox::TextAlign::topLeft     ); break;
	//	}
	//});
	//Button autoCheck(frame, 0, 0, 0, 0, L"autoCheck", [&] (Button::Event& ) {
	//	c.autoCheck(!c.autoCheck());
	//});
	//Button threeState(frame, 0, 0, 0, 0, L"threeState", [&] (Button::Event& ) {
	//	c.threeState(!c.threeState());
	//});
	//Button pushLike(frame, 0, 0, 0, 0, L"pushLike", [&] (Button::Event& ) {
	//	c.pushLike(!c.pushLike());
	//	c.resize();
	//});
	//Button rightButton(frame, 0, 0, 0, 0, L"rightButton", [&] (Button::Event& ) {
	//	c.rightButton(!c.rightButton());
	//});
	//Button state(frame, 0, 0, 0, 0, L"state", [&] (Button::Event& ) {
	//	int state = c.state();
	//	--state;
	//	if (state < 0) {
	//		state = 2;
	//	}
	//	c.state(static_cast<CheckBox::State>(state));
	//});
	//c.onStateChange() = [&] (CheckBox::StateChange& ) {
	//	Debug::writeLine(L"stateChanged");
	//	//int state = c.state();
	//	//--state;
	//	//if (state < 0) {
	//	//	state = 2;
	//	//}
	//	//c.state(static_cast<CheckBox::State>(state));
	//};




	//// Button 긡긚긣
	//Button cc(frame, 10, 20, 0, 0, L"control\nunko");
	//Button c(move(cc));
	//cc = move(c);
	//c = move(cc);
	//c.onClick() = [&] (Button::Event& ) {
	//	MsgBox::show(L"click");
	//};

	//Button multiline(frame, 0, 0, 0, 0, L"multiline", [&] (Button::Click& ) {
	//	c.multiline(!c.multiline());
	//});
	//Edit text0(frame, 0, 0, 0, 0, 20);
	//text0.text(c.text());
	//Button text(frame, 0, 0, 0, 0, L"text", [&] (Button::Click& ) {
	//	c.text(text0.text());
	//	c.resize();
	//	layout.perform();
	//});
	//auto onPaint0 = [&] (Button::Paint& e) {
	//	auto& g = e.graphics();
	//	if (e.sender().enabled()) {
	//		if (e.sender().pushed()) {
	//			g.brush(Color::blue());
	//		} else if (e.sender().hot()) {
	//			g.brush(Color::cyan());
	//		} else {
	//			g.brush(Color::green());
	//		}
	//	} else {
	//		g.brush(Color::gray());
	//	}
	//	auto r = e.sender().bounds();
	//	r.position(0, 0);
	//	g.drawRectangle(r);
	//	if (e.sender().focused()) {
	//		g.brush(Brush::hollow());
	//		r.grow(-5, -5);
	//		g.drawRectangle(r);
	//	}
	//};
	//Button onPaint(frame, 0, 0, 0, 0, L"onPaint", [&] (Button::Click& ) {
	//	if (c.onPaint()) {
	//		c.onPaint().pop();
	//	} else {
	//		c.onPaint() = onPaint0;
	//	}
	//	c.invalidate();
	//});
	//Button textAlign(frame, 0, 0, 0, 0, L"testAlign", [&] (Button::Event& ) {
	//	switch ((int)c.textAlign()) {
	//		case ButtonBase::TextAlign::topLeft      : c.textAlign(ButtonBase::TextAlign::topCenter   ); break;
	//		case ButtonBase::TextAlign::topCenter    : c.textAlign(ButtonBase::TextAlign::topRight    ); break;
	//		case ButtonBase::TextAlign::topRight     : c.textAlign(ButtonBase::TextAlign::middleLeft  ); break;
	//		case ButtonBase::TextAlign::middleLeft   : c.textAlign(ButtonBase::TextAlign::middleCenter); break;
	//		case ButtonBase::TextAlign::middleCenter : c.textAlign(ButtonBase::TextAlign::middleRight ); break;
	//		case ButtonBase::TextAlign::middleRight  : c.textAlign(ButtonBase::TextAlign::bottomLeft  ); break;
	//		case ButtonBase::TextAlign::bottomLeft   : c.textAlign(ButtonBase::TextAlign::bottomCenter); break;
	//		case ButtonBase::TextAlign::bottomCenter : c.textAlign(ButtonBase::TextAlign::bottomRight ); break;
	//		case ButtonBase::TextAlign::bottomRight  : c.textAlign(ButtonBase::TextAlign::topLeft     ); break;
	//	}
	//});

	//Button defaultButton(frame, 0, 0, 0, 0, L"defaultButton", [&] (Button::Click& ) {
	//	c.defaultButton(!c.defaultButton());
	//});
	//Button performClick(frame, 0, 0, 0, 0, L"performClick", [&] (Button::Click& ) {
	//	c.performClick();
	//});





//#include <Windows.h>
//#pragma comment(lib, "winmm.lib")
//
//	std::vector<Base*> bases;
//	bases.resize(1000);
//	std::srand(10);
//	for (auto i = bases.begin(); i != bases.end(); ++i) {
//		if (std::rand() < RAND_MAX) {
//			*i = new ClassA();
//		} else {
//			*i = new ClassB();
//		}
//	}
//
//
//	button0.click() += [&] (Event& ) {
//		timeBeginPeriod(1);
//		auto t = timeGetTime();
//
//		int count = 0;
//		for (int j = 0; j < 1000; ++j) {
//		for (auto i = bases.begin(), end = bases.end(); i != end; ++i) {
//			ClassA* a = dynamic_cast<ClassA*>(*i);
//			if (a) {
//				count += a->count;
//			}
//		}
//		}
//
//		t = timeGetTime() - t;
//		timeEndPeriod(1);
//		Debug::writeLine(String() + t + L":" + count);
//	};
//
//	button1.click() += [&] (Event& ) {
//		timeBeginPeriod(1);
//		auto t = timeGetTime();
//
//		int count = 0;
//		for (int j = 0; j < 1000; ++j) {
//		for (auto i = bases.begin(), end = bases.end(); i != end; ++i) {
//			Base* b = *i;
//			if (b->type == 0) {
//				count += ((ClassA*)b)->count;
//			}
//		}
//		}
//
//		t = timeGetTime() - t;
//		timeEndPeriod(1);
//		Debug::writeLine(String() + t + L":" + count);
//	};




//class Test {
//public:
//	Test() { Debug::writeLine(L"default construct"); }
//	Test(int i) { set(i); Debug::writeLine(L"with arg construct"); }
//	Test(Test&& test) : pimpl(std::move(test.pimpl)) { Debug::writeLine(L"rvalue construct"); }
//	~Test() { Debug::writeLine(L"destruct"); }
//
//	Test& operator=(Test&& test) { pimpl = std::move(test.pimpl); Debug::writeLine(L"rvalue operator="); return *this; }
//
//	int get() { return pimpl->i; }
//	void set(int i) { pimpl->i = i; }
//
//private:
//	Test(const Test& test) : pimpl(test.pimpl) { Debug::writeLine(L"copy construct"); }
//	Test& operator=(const Test& test) { pimpl = test.pimpl; Debug::writeLine(L"operator ="); return *this; }
//
//	class Impl {
//	public:
//		Impl() : i(2) {}
//
//		int i;
//	};
//	Pimpl<Impl> pimpl;
//};
