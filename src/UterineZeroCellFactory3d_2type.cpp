#include "UterineZeroCellFactory3d_2type.hpp"
#include "Exception.hpp"
#include <cstdlib>


UterineZeroCellFactory3d_2type::UterineZeroCellFactory3d_2type() : 
	AbstractUterineCellFactory3d_2type(), 
	mpStimulus(new ZeroStimulus())
{
	//ReadParams(USMC_3D_SYSTEM_CONSTANTS::GENERAL_PARAM_FILE);
	ReadParams(USMC_3D_2TYPE_SYSTEM_CONSTANTS::GENERAL_2TYPE_PARAM_FILE);
	//ReadCellParams(mpCell_type + ".toml");
}


AbstractCvodeCell* UterineZeroCellFactory3d_2type::CreateCardiacCellForTissueNode(
	Node<3>* pNode)
{

	double x = pNode->rGetLocation()[0];
	double y = pNode->rGetLocation()[1];
	double z = pNode->rGetLocation()[2];

	//attempt generate random
	double myrand = rand() % 10000;
	myrand = myrand/10000;

	//std::cout << "UterineZeroCellFactory3d_2type: myrand: " << myrand << std::endl;
	//std::cout << "UterineZeroCellFactory3d_2type: myrand: " <<	std::format("{}", myrand);


	//mp_nodes_cell1 = 0.0;
	//mp_nodes_cell2 = 0.0;

	AbstractCvodeCell* cell;	

	//std::cout << "UterineZeroCellFactory3d_2type: node loop..." << std::endl;

/* 	if (x >= mpX_passive_start && x <= mpX_passive_end && 
			y >= mpY_passive_start && y <= mpY_passive_end &&
			z >= mpZ_passive_start && z <= mpZ_passive_end) */
	if (x >= mpX_passive_start && x <= mpX_passive_end && 
			y >= mpY_passive_start && y <= mpY_passive_end &&
			z >= mpZ_passive_start && z <= mpZ_passive_end &&
			myrand < mp_prob_passive)


	{

		//mp_nodes_cell2++;

		switch (mpCell2_id) //testing: set here to the 2nd cell type => all parms below set to 'mpCell2_parameters'
		{
			case 0:
				cell = new CellHodgkinHuxley1952FromCellMLCvode(mpSolver,
					mpZeroStimulus);
				break;

			case 1:
				cell = new CellChayKeizer1983FromCellMLCvode(mpSolver, mpZeroStimulus);
				break;

			case 2:
				cell = new CellMeans2022FromCellMLCvode(mpSolver, mpZeroStimulus);

				for (auto it=mpCell2_parameters.begin(); it != mpCell2_parameters.end(); ++it)
				{
					cell->SetParameter(it->first, it->second);
				}
				break;
			
			case 3:
				cell = new CellTong2014FromCellMLCvode(mpSolver, mpZeroStimulus);

				for (auto it=mpCell2_parameters.begin(); it != mpCell2_parameters.end(); ++it)
				{
					cell->SetParameter(it->first, it->second);
				}
				break;

			case 4:
				cell = new CellRoesler2023FromCellMLCvode(mpSolver, mpZeroStimulus);

				for (auto it=mpCell2_parameters.begin(); it != mpCell2_parameters.end(); ++it)
				{
					cell->SetParameter(it->first, it->second);
				}
				break;

			case 5:
				cell = new Celliclc_passive_cell_v1FromCellMLCvode(mpSolver, mpStimulus);

				//nodes_cell2++;

				for (auto it=mpCell2_parameters.begin(); it != mpCell2_parameters.end(); ++it)
				{
					cell->SetParameter(it->first, it->second);
				}
				break;								

			default:
				cell = new CellHodgkinHuxley1952FromCellMLCvode(mpSolver,
					mpStimulus);
		}
		return cell;
	}
	else
	{
		//mp_nodes_cell1++;
		return AbstractUterineCellFactory3d_2type::CreateCardiacCellForTissueNode(pNode);
	}
}


void UterineZeroCellFactory3d_2type::ReadParams(std::string general_param_file)
{
	AbstractUterineCellFactory3d_2type::ReadParams(general_param_file);


	std::string general_param_path = USMC_3D_2TYPE_SYSTEM_CONSTANTS::CONFIG_DIR +
		general_param_file;
	const auto params = toml::parse(general_param_path);
	
	// passive cell location parameters
	mpX_passive_start = toml::find<double>(params, "x_passive_start");
	mpX_passive_end = toml::find<double>(params, "x_passive_end");
	mpY_passive_start = toml::find<double>(params, "y_passive_start");
	mpY_passive_end = toml::find<double>(params, "y_passive_end");
	mpZ_passive_start = toml::find<double>(params, "z_passive_start");
	mpZ_passive_end = toml::find<double>(params, "z_passive_end");	

	//passive cell density probability
	mp_prob_passive = toml::find<double>(params, "prob_passive");	

	//initialise node counts?
	//mp_nodes_cell1 = 0.0;
	//mp_nodes_cell2 = 0.0;

	//note no stimulus parms expected nor read...
}


/* void UterineZeroCellFactory3d_2type::ReadCellParams(std::string cell_param_file)
{
	AbstractUterineCellFactory3d_2type::ReadCellParams(cell_param_file);
}
 */

void UterineZeroCellFactory3d_2type::PrintParams()
{
	AbstractUterineCellFactory3d_2type::PrintParams();

	std::cout << "mpX_passive_start = " << mpX_passive_start << "\n";
	std::cout << "mpX_passive_end = " << mpX_passive_end << "\n";
	std::cout << "mpY_passive_start = " << mpY_passive_start << "\n";
	std::cout << "mpY_passive_end = " << mpY_passive_end << "\n";
	std::cout << "mpZ_passive_start = " << mpZ_passive_start << "\n";
	std::cout << "mpZ_passive_end = " << mpZ_passive_end << "\n";	

	std::cout << "mp_prob_passive = " << mp_prob_passive << "\n";

	//std::cout << "Total nodes / cell1 / cell2: " << mp_nodes_cell1 + mp_nodes_cell2 << " / " <<
	//	mp_nodes_cell1 << " / " << mp_nodes_cell2 << std::endl;
}


void UterineZeroCellFactory3d_2type::WriteLogInfo(std::string log_file)
{
	AbstractUterineCellFactory3d_2type::WriteLogInfo(log_file);

	std::ofstream log_stream;
	log_stream.open(log_file, ios::app); // Open log file in append mode

	log_stream << "Stimulus parameters" << std::endl;
	log_stream << "  type: zero" << std::endl;

	log_stream << "  passive region: " << mpX_passive_start << " <= x <= ";
	log_stream << mpX_passive_end << "   " << mpY_passive_start << " <= y <= ";
	log_stream << mpY_passive_end << "   " << mpZ_passive_start << " <= z <= ";
	log_stream << mpZ_passive_end << std::endl;

	log_stream << "prob passive nodes: " << mp_prob_passive << std::endl;

	log_stream.close();
}

