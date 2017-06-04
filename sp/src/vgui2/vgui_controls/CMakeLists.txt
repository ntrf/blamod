# 
# Copyright 2016 - 2017 Nesterov A.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http ://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include_directories(
	"../../common"
	"../../public"
	"../../public/tier1"
	"../../public/tier0"
)

add_definitions(
	"-D_HAS_ITERATOR_DEBUGGING=0" "-DRAD_TELEMETRY_DISABLED"
)

add_library(vgui_controls STATIC
    ../../public/vgui_controls/AnalogBar.h
    ../../public/vgui_controls/AnimatingImagePanel.h
    ../../public/vgui_controls/AnimationController.h
    ../../public/vgui_controls/BitmapImagePanel.h
    ../../public/vgui_controls/BuildGroup.h
    ../../public/vgui_controls/BuildModeDialog.h
    ../../public/vgui_controls/Button.h
    ../../public/vgui_controls/CheckButton.h
    ../../public/vgui_controls/CheckButtonList.h
    ../../public/vgui_controls/CircularProgressBar.h
    ../../public/Color.h
    ../../public/vgui_controls/ComboBox.h
    ../../public/vgui_controls/consoledialog.h
    ../../public/vgui_controls/ControllerMap.h
    ../../public/vgui_controls/Controls.h
    ../../public/vgui_controls/CvarToggleCheckButton.h
    ../../public/vgui_controls/DialogManager.h
    ../../public/vgui_controls/DirectorySelectDialog.h
    ../../public/vgui_controls/Divider.h
    ../../public/vgui_controls/EditablePanel.h
    ../../public/vgui_controls/ExpandButton.h
    ../../public/vgui_controls/FileOpenDialog.h
    ../../public/vgui_controls/FileOpenStateMachine.h
    ../../public/filesystem.h
    ../../public/filesystem_helpers.h
    ../../public/vgui_controls/FocusNavGroup.h
    ../../public/vgui_controls/Frame.h
    ../../public/vgui_controls/GraphPanel.h
    ../../public/vgui_controls/Image.h
    ../../public/vgui_controls/ImageList.h
    ../../public/vgui_controls/ImagePanel.h
    ../../public/vgui_controls/InputDialog.h
    ../../public/tier1/interface.h
    ../../public/vgui_controls/KeyBindingHelpDialog.h
    ../../public/vgui_controls/KeyBindingMap.h
    ../../public/vgui_controls/KeyBoardEditorDialog.h
    ../../public/vgui_controls/KeyRepeat.h
    ../../public/tier1/KeyValues.h
    ../../public/vgui_controls/Label.h
    ../../public/vgui_controls/ListPanel.h
    ../../public/vgui_controls/ListViewPanel.h
    ../../public/tier0/memdbgoff.h
    ../../public/tier0/memdbgon.h
    ../../public/tier1/mempool.h
    ../../public/vgui_controls/Menu.h
    ../../public/vgui_controls/MenuBar.h
    ../../public/vgui_controls/MenuButton.h
    ../../public/vgui_controls/MenuItem.h
    ../../public/vgui_controls/MessageBox.h
    ../../public/vgui_controls/MessageDialog.h
    ../../public/vgui_controls/MessageMap.h
    ../../public/vgui_controls/Panel.h
    ../../public/vgui_controls/PanelAnimationVar.h
    ../../public/vgui_controls/PanelListPanel.h
    ../../public/vgui_controls/PerforceFileExplorer.h
    ../../public/vgui_controls/PerforceFileList.h
    ../../public/vgui_controls/perforcefilelistframe.h
    ../../public/vgui_controls/PHandle.h
    ../../public/vgui_controls/ProgressBar.h
    ../../public/vgui_controls/ProgressBox.h
    ../../public/vgui_controls/PropertyDialog.h
    ../../public/vgui_controls/PropertyPage.h
    ../../public/vgui_controls/PropertySheet.h
    ../../public/vgui_controls/QueryBox.h
    ../../public/vgui_controls/RadioButton.h
    ../../public/vgui_controls/RichText.h
    ../../public/vgui_controls/RotatingProgressBar.h
    ../../public/vgui_controls/savedocumentquery.h
    ../../public/vgui_controls/ScalableImagePanel.h
    ../../public/vgui_controls/ScrollableEditablePanel.h
    ../../public/vgui_controls/ScrollBar.h
    ../../public/vgui_controls/ScrollBarSlider.h
    ../../public/vgui_controls/SectionedListPanel.h
    ../../public/vgui_controls/Slider.h
    ../../public/vgui_controls/Splitter.h
    ../../public/vgui_controls/subrectimage.h
    ../../public/vgui_controls/TextEntry.h
    ../../public/vgui_controls/TextImage.h
    ../../public/vgui_controls/ToggleButton.h
    ../../public/vgui_controls/Tooltip.h
    ../../public/vgui_controls/ToolWindow.h
    ../../public/vgui_controls/TreeView.h
    ../../public/vgui_controls/TreeViewListControl.h
    ../../public/vgui_controls/URLLabel.h
    ../../public/tier1/utlmemory.h
    ../../public/tier1/utlrbtree.h
    ../../public/tier1/utlvector.h
    ../../public/vgui_controls/WizardPanel.h
    ../../public/vgui_controls/WizardSubPanel.h
	
    AnalogBar.cpp
    AnimatingImagePanel.cpp
    AnimationController.cpp
    BitmapImagePanel.cpp
    BuildFactoryHelper.cpp
    BuildGroup.cpp
    BuildModeDialog.cpp
    Button.cpp
    CheckButton.cpp
    CheckButtonList.cpp
    CircularProgressBar.cpp
    ComboBox.cpp
    consoledialog.cpp
    ControllerMap.cpp
    controls.cpp
    CvarToggleCheckButton.cpp
    DirectorySelectDialog.cpp
    Divider.cpp
    EditablePanel.cpp
    ExpandButton.cpp
    FileOpenDialog.cpp
    FileOpenStateMachine.cpp
    ../../public/filesystem_helpers.cpp
    FocusNavGroup.cpp
    Frame.cpp
    GraphPanel.cpp
    # HTML.cpp
    Image.cpp
    ImageList.cpp
    ImagePanel.cpp
    InputDialog.cpp
    KeyBindingHelpDialog.cpp
    KeyBoardEditorDialog.cpp
    KeyRepeat.cpp
    Label.cpp
    ListPanel.cpp
    ListViewPanel.cpp
    Menu.cpp
    MenuBar.cpp
    MenuButton.cpp
    MenuItem.cpp
    MessageBox.cpp
    MessageDialog.cpp
    Panel.cpp
    PanelListPanel.cpp
    PerforceFileExplorer.cpp
    PerforceFileList.cpp
    perforcefilelistframe.cpp
    ProgressBar.cpp
    ProgressBox.cpp
    PropertyDialog.cpp
    PropertyPage.cpp
    PropertySheet.cpp
    QueryBox.cpp
    RadioButton.cpp
    RichText.cpp
    RotatingProgressBar.cpp
    savedocumentquery.cpp
    ScalableImagePanel.cpp
    ScrollableEditablePanel.cpp
    ScrollBar.cpp
    ScrollBarSlider.cpp
    SectionedListPanel.cpp
    Slider.cpp
    Splitter.cpp
    subrectimage.cpp
    TextEntry.cpp
    TextImage.cpp
    ToggleButton.cpp
    Tooltip.cpp
    ToolWindow.cpp
    TreeView.cpp
    TreeViewListControl.cpp
    URLLabel.cpp
    ../src/vgui_key_translation.cpp
    WizardPanel.cpp
    WizardSubPanel.cpp
)

if (MSVC)
#	target_link_libraries(vgui_controls
#		"Rpcrt4.lib" "odbccp32.lib"
#	)
endif()

