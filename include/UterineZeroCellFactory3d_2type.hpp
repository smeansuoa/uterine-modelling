#ifndef UTERINEZEROCELLFACTORY3D_2TYPE_HPP
#define UTERINEZEROCELLFACTORY3D_2TYPE_HPP

#include <toml.hpp>
#include "AbstractUterineCellFactory3d_2type.hpp"
#include "MonodomainProblem.hpp"
#include "ZeroStimulus.hpp"

class UterineZeroCellFactory3d_2type : public AbstractUterineCellFactory3d_2type
{
private:
	double mpX_passive_start;
	double mpX_passive_end;
	double mpY_passive_start;
	double mpY_passive_end;
	double mpZ_passive_start;
	double mpZ_passive_end;

	double mp_prob_passive;
	double mp_nodes_cell1;
	double mp_nodes_cell2;

	boost::shared_ptr<ZeroStimulus> mpStimulus;

public:
	UterineZeroCellFactory3d_2type();
	AbstractCvodeCell* CreateCardiacCellForTissueNode(Node<3>* pNode);
	void ReadParams(std::string general_param_file);
	//void ReadCellParams(std::string cell_param_file);
	void PrintParams();
	void WriteLogInfo(std::string log_file);
};

#endif // UTERINEZEROCELLFACTORY3D_2TYPE_HPP



