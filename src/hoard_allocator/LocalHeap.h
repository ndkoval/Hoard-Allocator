#ifndef LOCAL_HEAP_H
#define LOCAL_HEAP_H

#include <array>
#include "BaseHeap.h"

namespace hoard {


class LocalHeap : public BaseHeap {

constexpr static size_t kBinCount = 7; //blocks_allocated / size = 0,  <=1/8, <=1/4, <=2/4, <=3/4, <1, 1
constexpr static size_t kAlmostFullBlocksBinNum = 6; // 3/4<  blocks_allocated / size <1
constexpr static size_t kEmptySuperblocksBinNum = 0; // blocks_allocated / size = 0

public:
	LocalHeap(GlobalHeap *parent_heap) : parent_heap_(parent_heap), blocks_allocated_(0), blocks_size_(0) {
	}

	void * Alloc() {
		lock_guard guard(lock);
		if (blocks_allocated_ == blocks_size_) {
			GetSuperblock();
		}

		for(int i_bin = kAlmostFullBlocksBinNum; i_bin >= 0; --i_bin) {
			SuperblockStack & bin = bins_[i_bin];
			if (!bin.IsEmpty()) {
				SuperblockHeader & header = bin.Top()->header();
				void * allocated =  header.Alloc();
				const int new_bin_num = BinNum(header.blocks_allocated(), header.block_size());
				if (new_bin_num != i_bin) {
					Superblock *superblock = bin.Pop();
					bins_[new_bin_num].Push(superblock);
				}
				return allocated;
			}
		}
		assert(false && "no superblock choosed, shouldn't happen");
	}

	virtual void OnFreeSuperblock(Superblock *superblock) override {
		SuperblockHeader & header = superblock->header();
//		const size_t old_bin = BinNum(header.a)

	}



private:
	GlobalHeap * const parent_heap_;
	std::array<SuperblockStack, kBinCount> bins_;
	size_t blocks_allocated_;
	size_t blocks_size_;

	void GetSuperblock() {

	}

	void TransferSuperblock() {

	}

	size_t BinNum(size_t blocs_allocated, size_t blocs_size) {
		switch (RoundUp(blocs_allocated * 8, blocs_size)) {
			case 0: return 0; // e == 0
			case 1: return 1; // x <= 1/8
			case 2: return 2; // <= 1/4
			case 3:
			case 4: return 4; // <= 2/4
			case 5:
			case 6: return 5; // <= 3/4
			case 7:
			case 8: return blocs_allocated == blocs_size ? 7 : 6;
		}
		assert(false && "invalid bin num");
	}
};

}
#endif // LOCAL_HEAP_H
