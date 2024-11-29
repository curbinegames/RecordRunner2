#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sancur.h>

template<typename T = int>
class cur_datalist_c {
private:
	T *data; /* データ本体、mallocで取得している */
	const uint count; /* dataの配列数 */
	uint nownum = 0; /* 今、参照している配列番号 */

	/**
	 * @return true = error, false = safe
	 */
	inline bool ErrorCheckNum(int num) const {
		return (num < 0 && this->count <= (uint)num) ? (true) : (false);
	}

public:
	cur_datalist_c(uint num, bool zeroinit = false) : count(num) {
		this->data = (T *)malloc(sizeof(T) * num);
		if (this->data == NULL) { return; }
		if (zeroinit) {
			memset(this->data, 0, sizeof(T) * num);
		}
		else {
			T initVal;
			for (uint i = 0; i < num; i++) { this->data[i] = initVal; }
		}
		return;
	}

	~cur_datalist_c() {
		if (this->data == NULL) { return; }
		free(this->data);
		return;
	}

	T GetVal(int offset = 0) const {
		int viewnum = this->nownum + offset;
		if (this->ErrorCheckNum(viewnum)) {
			T errorval;
			return errorval;
		}
		return this->data[viewnum];
	}

	void SetVal(T val, int offset = 0) {
		int viewnum = (int)this->nownum + offset;
		if (this->ErrorCheckNum(viewnum)) { return; }
		this->data[viewnum] = val;
		return;
	}

	void SeekNum(int offset, int origin = SEEK_CUR) {
		int setRes = 0;
		switch (origin) {
		case SEEK_SET:
			setRes = offset;
			break;
		case SEEK_CUR:
			setRes = (int)this->nownum + offset;
			break;
		case SEEK_END:
			setRes = (int)this->count - 1 + offset;
			break;
		default:
			setRes = (int)this->nownum;
			break;
		}
		if (this->ErrorCheckNum(setRes)) { return; }
		this->nownum = (uint)setRes;
		return;
	}

#if 1 /* operator */
	cur_datalist_c &operator +(const int &r) {
		this->SeekNum(r);
		return *this;
	}

	cur_datalist_c &operator -(const int &r) {
		this->SeekNum(-r);
		return *this;
	}

	cur_datalist_c &operator ++() {
		this->SeekNum(1);
		return *this;
	}

	cur_datalist_c &operator ++(int) {
		this->SeekNum(1);
		return *this;
	}

	cur_datalist_c &operator --() {
		this->SeekNum(-1);
		return *this;
	}

	cur_datalist_c &operator --(int) {
		this->SeekNum(-1);
		return *this;
	}

	T &operator [](size_t index) const {
		return data[index];
	}

	cur_datalist_c &operator =(const cur_datalist_c &) = delete;
#endif /* operator */
};
