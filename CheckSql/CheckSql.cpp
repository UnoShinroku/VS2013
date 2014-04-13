// CheckSql.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	FILE	*ifp;
	FILE	*ofp;
	int		num;
	int		nLine;
	int		nPos;
	int		nType;
	int		nType2;
	LPSTR	lpToken;
	LPCSTR	lpDelim = "\t\n";
	TCHAR	szLine[50000];
	TCHAR	szLine2[50000];
	BOOL	isLoop;
	BOOL	isValid;

	if(argc != 3) {
		printf("\n引数1: 入力ファイル名");
		printf("\n引数2: 出力ファイル名\n");
		return 0;
	}

	nLine = 0;
	ifp = fopen(argv[1], "r");
	ofp = fopen(argv[2], "wb");
	if(ifp != NULL && ofp != NULL) {
		num = sizeof(szLine);
		while(fgets(szLine, num, ifp) != NULL) {
			nLine++;
			strcpy(szLine2, szLine);
			isValid = FALSE;
			lpToken = strtok(szLine2, lpDelim);
			while(lpToken != NULL && isValid == FALSE) {
				if(*lpToken != '\\') {
					nPos = 0;
					isLoop = TRUE;
					nType2 = _MBC_SINGLE;
					while(isLoop) {
						nType = _mbsbtype((unsigned char *)lpToken, nPos); 
						switch(nType) {
						case _MBC_SINGLE:
							switch(*(lpToken + nPos)) {
							case '\\':
								if(nType2 == _MBC_TRAIL) {
									printf("%8d : (￥)[%s]\n", nLine, lpToken);
									isLoop = FALSE;
									isValid = TRUE;
								}
								break;
							default:
								break;
							}
							break;
						case _MBC_LEAD:
							if((UINT)*(lpToken + nPos) >= 0xF0 && (UINT)*(lpToken + nPos) < 0xFA) {
								printf("%8d : (外)[%s]\n", nLine, lpToken);
							}
							break;
						case _MBC_TRAIL:
							break;
						default:
							switch(*(lpToken + nPos)) {
							case '\0':
								isLoop = FALSE;
								break;
							default:
								printf("%8d : (異)[%s]\n", nLine, lpToken);
								isLoop = FALSE;
								isValid = TRUE;
								break;
							}
							break;
						}
						nType2 = nType;
						nPos++;
					}
				}
				lpToken = strtok(NULL, lpDelim);
			}
			if(isValid) {
				nPos = 0;
				nType2 = _MBC_SINGLE;
				isLoop = TRUE;
				while(isLoop) {
					nType = _mbsbtype((unsigned char *)szLine, nPos); 
					switch(nType) {
					case _MBC_SINGLE:
						switch(szLine[nPos]) {
						case '\\':
							// (￥)は除外する
							if(nType2 != _MBC_TRAIL) {
								fwrite(szLine + nPos, 1, 1, ofp);
							}
							break;
						default:
							fwrite(szLine + nPos, 1, 1, ofp);
							break;
						}
						break;
					case _MBC_LEAD:
					case _MBC_TRAIL:
						// (外)はそのまま出力する
						fwrite(szLine + nPos, 1, 1, ofp);
						break;
					default:
						switch(szLine[nPos]) {
						case '\0':
							isLoop = FALSE;
							break;
						case '\t':
						case '\n':
							fwrite(szLine + nPos, 1, 1, ofp);
							break;
						default:
							// (異)は?に置換する
							fwrite("?", 1, 1, ofp);
							break;
						}
						break;
					}
					nType2 = nType;
					nPos++;
				}
			} else {
				fputs(szLine, ofp);
			}
		}
		fclose(ifp);
		fclose(ofp);
	}
	return 0;
}

