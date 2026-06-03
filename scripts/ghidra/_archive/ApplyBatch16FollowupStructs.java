// Apply CListBoxItem (0x14), CSessionList CWindow base, CSessionItem base rename (batch 16 follow-up).
//@category Bulanci.StructRecovery

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;

public class ApplyBatch16FollowupStructs extends GhidraScript {

	@Override
	public void run() throws Exception {
		DataTypeManager dtm = currentProgram.getDataTypeManager();

		DataType oldLb = dtm.getDataType("/CListBoxItem");
		if (oldLb != null) {
			dtm.remove(oldLb, null);
		}

		StructureDataType lb = new StructureDataType("CListBoxItem", 0);
		lb.add(POINTER, 4, "pVftable", null);
		lb.add(DWORD, 4, "dwField_04", null);
		lb.add(DWORD, 4, "pAssocOrNext", null);
		lb.add(DWORD, 4, "rowFlags", null);
		lb.add(DWORD, 4, "labelStringHandle", null);
		lb.setCategoryPath(new CategoryPath("/"));
		dtm.addDataType(lb, DataTypeConflictHandler.REPLACE_HANDLER);

		DataType oldSl = dtm.getDataType("/CSessionList");
		if (oldSl != null) {
			dtm.remove(oldSl, null);
		}

		StructureDataType sl = new StructureDataType("CSessionList", 0);
		sl.add(POINTER, 4, "pVftable_primary", null);
		sl.add(POINTER, 4, "pVftable_IDSChained", null);
		sl.add(DWORD, 4, "dwField_08", null);
		sl.add(DWORD, 4, "dwField_0c", null);
		sl.add(POINTER, 4, "pVftable_IDSEventHandler", null);
		sl.add(WORD, 2, "wViewFlags", null);
		sl.add(new ArrayDataType(BYTE, 2, 1), 2, "wPad_16", null);
		sl.add(POINTER, 4, "pVftable_field18", null);
		sl.add(DWORD, 4, "dwField_1c", null);
		sl.add(DWORD, 4, "nBbox_left", null);
		sl.add(DWORD, 4, "nBbox_top", null);
		sl.add(DWORD, 4, "nBbox_right", null);
		sl.add(DWORD, 4, "nBbox_bottom", null);
		sl.add(DWORD, 4, "dwField_30", null);
		sl.add(DWORD, 4, "dwField_34", null);
		sl.add(DWORD, 4, "dwField_38", null);
		sl.add(DWORD, 4, "dwField_3c", null);
		sl.add(new ArrayDataType(BYTE, 6, 1), 6, "pad_40_45", null);
		sl.add(WORD, 2, "wContainerFlags", null);
		sl.add(new ArrayDataType(BYTE, 4, 1), 4, "pad_48_4b", null);
		sl.add(POINTER, 4, "pParent", null);
		sl.add(new ArrayDataType(BYTE, 4, 1), 4, "pad_50_53", null);
		sl.add(POINTER, 4, "pVftable_CDSChain_IDSReferenced", null);
		sl.add(POINTER, 4, "pVftable_CDSChain_IDSChained", null);
		sl.add(DWORD, 4, "dwField_5c", null);
		sl.add(DWORD, 4, "dwField_60", null);
		sl.add(DWORD, 4, "dwField_64", null);
		sl.add(BYTE, 1, "bModalFlag", null);
		sl.add(new ArrayDataType(BYTE, 3, 1), 3, "pad_69_6b", null);
		sl.add(POINTER, 4, "pDefaultFocusChild", null);
		DataType pListBox = dtm.getDataType("/CListBox");
		if (pListBox == null) {
			pListBox = POINTER;
		}
		sl.add(pListBox, 4, "pSessionListBox", null);
		DataType pStatic = dtm.getDataType("/CStaticText");
		if (pStatic == null) {
			pStatic = POINTER;
		}
		sl.add(pStatic, 4, "pCaptionStatic", null);
		DataType pBtn = dtm.getDataType("/CButton");
		if (pBtn == null) {
			pBtn = POINTER;
		}
		sl.add(pBtn, 4, "pJoinButton", null);
		sl.setCategoryPath(new CategoryPath("/"));
		dtm.addDataType(sl, DataTypeConflictHandler.REPLACE_HANDLER);

		DataType oldSi = dtm.getDataType("/CSessionItem");
		if (oldSi != null) {
			dtm.remove(oldSi, null);
		}

		StructureDataType si = new StructureDataType("CSessionItem", 0);
		si.add(POINTER, 4, "pVftable", null);
		si.add(DWORD, 4, "dwField_04", null);
		si.add(DWORD, 4, "pAssocOrNext", null);
		si.add(DWORD, 4, "rowFlags", null);
		si.add(DWORD, 4, "labelStringHandle", null);
		si.add(DWORD, 4, "hostIp", null);
		si.add(DWORD, 4, "port", null);
		si.add(DWORD, 4, "playerCount", null);
		si.add(DWORD, 4, "sessionFlags", null);
		si.setCategoryPath(new CategoryPath("/"));
		dtm.addDataType(si, DataTypeConflictHandler.REPLACE_HANDLER);

		printf("CListBoxItem size=%d\n", lb.getLength());
		printf("CSessionList size=%d\n", sl.getLength());
		printf("CSessionItem size=%d\n", si.getLength());
	}
}
