#ifndef CLIPPINGCOHENSUTHERLAND_H_
#define CLIPPINGCOHENSUTHERLAND_H_

#include "gui/clipping/Clipping.h"

/**
 * Recorte de objetos de Cohen-Sutherland.
 */
class ClippingCohenSutherland : public Clipping {

public:
	/**
	 * Construtor.
	 * @param xvMin x mínimo da viewport.
	 * @param xvMax x máximo da viewport.
	 * @param yvMin y mínimo da viewport.
	 * @param yvMax y máximo da viewport.
	 */
	ClippingCohenSutherland(const double xvMin, const double xvMax, const double yvMin, const double yvMax);

	/**
	 * Destrutor.
	 */
	virtual ~ClippingCohenSutherland();

protected:
	virtual bool clipReta(Reta* const reta) const;

private:
	bool clippingDireita(Ponto *p, const double coefAngular) const;
	bool clippingEsquerda(Ponto *p, const double coefAngular) const;
	bool clippingFundo(Ponto *p, const double coefAngular) const;
	bool clippingTopo(Ponto *p, const double coefAngular) const;

};

#endif /* CLIPPINGCOHENSUTHERLAND_H_ */