
fn export#0 @ 0x0000  ; GetInfo(language)  -- writes globals 0/1/2 = name/type/GUID for the level/help/history picker
  ; varCount=0
    @0x0001  HhAddChild(HhSetStaticTextStyle(HhBuildStaticTextAuto(IntConst(30), IntConst(30), StrConst("1981-1983"), IntConst(1), 65711), count=2, [IntConst(0), IntConst(16777215)]))
    @0x0037  HhAddChild(HhBuildStaticTextAuto(IntConst(100), IntConst(30), StrConst("Rozvoj bulánčí poezie a prózy."), IntConst(1), 65711))
    @0x008b  HhAddChild(HhSetStaticTextStyle(HhBuildStaticTextAuto(IntConst(30), IntConst(50), StrConst("1982-1985"), IntConst(1), 65711), count=2, [IntConst(0), IntConst(16777215)]))
    @0x00c1  HhAddChild(HhBuildStaticTextAuto(IntConst(100), IntConst(50), StrConst("Hospodářská krize. Začíná se projevovat nedostatek základních potravin a léků.\nVelkou ranou pro bulánčí rasu je rovněž epidemie husího moru."), IntConst(1), 65711))
    @0x01f1  HhAddChild(HhSetStaticTextStyle(HhBuildStaticTextAuto(IntConst(30), IntConst(83), StrConst("12. 7. 1986"), IntConst(1), 65711), count=2, [IntConst(0), IntConst(16777215)]))
    @0x022b  HhAddChild(HhBuildStaticTextAuto(IntConst(100), IntConst(83), StrConst("Zavraždění prezidenta. V noci z 11. na 12.\nčervence je zavražděn umírněný vůdce Bulánků\nŽezlík. Vlády se pokouší zmocnit fanatický\ngenerál Pfühl, jež hlásá nadřazenost rasy s\nbílým draným peřím."), IntConst(1), 65711))
    @0x03c7  HhAddChild(HhSetStaticTextStyle(HhBuildStaticTextAuto(IntConst(30), IntConst(158), StrConst("18. 7. 1986"), IntConst(1), 65711), count=2, [IntConst(0), IntConst(16777215)]))
    @0x0401  HhAddChild(HhBuildStaticTextAuto(IntConst(100), IntConst(158), StrConst("Druhá polštářová válka. Rozpoutává se druhá,\nmnohem krutější a zákeřnější válka. Válka mezi\nBulánky samotnými. Na všech frontách dochází k\nvelkým ztrátám, které plodí stále větší nenávist,\nroztáčející nekonečnou spirálu krvelačné pomsty.\nHeslo \"Oko za oko\" zasadilo tehdy kořeny tomu,\nže se bulánčí rody dodnes neusmířily."), IntConst(1), 65711))
    @0x069d  HhAddChild(HCreateMovie(IntConst(30), IntConst(266), 65652, IntConst(77826), IntConst(77827)))
    @0x06b7  HhAddChild(HCreateMovie(IntConst(350), IntConst(90), 65605, IntConst(77824), IntConst(77825)))
    @0x06d1  HhAddChild(HhSetStaticTextStyle(HhBuildStaticTextAuto(IntConst(206), IntConst(276), StrConst("24. 8. 1988"), IntConst(1), 65711), count=2, [IntConst(0), IntConst(16777215)]))
    @0x070b  HhAddChild(HhBuildStaticTextAuto(IntConst(276), IntConst(276), StrConst("Olympijské hry. Bulánek Mazlík získává zlatou\nmedaili v letních olympijských hrách za střelbu\nna pohyblivý cíl. Na nějakou dobu tato událost\nsjednocuje znesvářené rody."), IntConst(1), 65711))
    @0x0873  HhAddChild(HhSetStaticTextStyle(HhBuildStaticTextAuto(IntConst(206), IntConst(335), StrConst("20. 3. 1992"), IntConst(1), 65711), count=2, [IntConst(0), IntConst(16777215)]))
    @0x08ad  HhAddChild(HhBuildStaticTextAuto(IntConst(276), IntConst(335), StrConst("První transfuze. Bulánkovi Kapslíkovi je\nprovedena první úspěšná transfuze peří."), IntConst(1), 65711))
    @0x0965  Return(IntConst(0))
